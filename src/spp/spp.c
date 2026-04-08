/**
 * @file src/spp/spp.c
 * @brief Space Packet Protocol (SPP) implementation.
 * This file contains the core logic for building and parsing CCSDS packets,
 * including sequence counter management and segmentation reassembly.
 *
 * @author astrobyte
 * @date 2026-04-08
 * @license GNU General Public License
 * @copyright Copyright (c) 2026 kevin Leon
 * @contact kevinleon.morales@gmail.com
 */

#include <common.h>
#include <spp/spp.h>
#include <string.h>

/**
 * @brief Validates and wraps sequence counters to 14 bits (max 16383).
 * @param[in,out] spp_apid_context Context containing counters to validate.
 */
static void spp_validate_counters(spp_apid_context_t *spp_apid_context) {
  if (spp_apid_context->tc >= 16383) {
    spp_apid_context->tc = 0;
  }
  if (spp_apid_context->tm >= 16383) {
    spp_apid_context->tm = 0;
  }
}

/**
 * @brief Internal generic packet builder.
 * * Handles bit-shifting and Big-Endian conversion for the CCSDS Primary
 * Header.
 */
static int spp_build_packet(space_packet_t *space_packet, const uint8_t type,
                            const uint8_t flag, const uint8_t sec_header,
                            const uint16_t sec_header_len, const uint16_t apid,
                            const uint16_t sequence_count, const uint8_t *data,
                            uint16_t data_len) {
  if (data_len == 0) {
    return SPP_ERROR_PAYLOAD_LEN;
  }
  if (apid > 0x07FF) {
    return SPP_ERROR_INVALID_APID;
  }
  if (apid == SPP_APID_IDLE && type != SPP_PTYPE_TM) {
    return SPP_ERROR_INVALID_APID;
  }
  if (data_len > 0 && data == NULL) {
    return SPP_ERROR_INVALID_BUFFER;
  }
  if (data_len > sizeof(space_packet->data)) {
    return SPP_ERROR_PAYLOAD_LEN;
  }
  if ((data_len + sec_header_len) > (SPP_MAX_PAYLOAD_CHUNK + 1)) {
    return SPP_ERROR_PAYLOAD_LEN;
  }
  if (sec_header == SPP_SECHEAD_FLAG_PRESENT && sec_header_len == 0) {
    return SPP_ERROR_MALFORMED_SEC_HEADER;
  }

  memset(space_packet, 0, sizeof(space_packet_t));

  uint16_t packet_id = 0;
  packet_id |= (CCSDS_SPP_VERSION & 0x07) << 13;
  packet_id |= (type & 0x01) << 12;
  packet_id |= (sec_header & 0x01) << 11;
  packet_id |= apid;

  uint16_t seq_ctrl = 0;
  seq_ctrl |= (flag & 0x03) << 14;
  seq_ctrl |= (sequence_count & 0x3FFF);

  space_packet->header.identification = HOST_TO_BE16(packet_id);
  space_packet->header.sequence = HOST_TO_BE16(seq_ctrl);
  space_packet->header.length = HOST_TO_BE16(data_len + sec_header_len - 1);

  memcpy(space_packet->data, data, data_len);

  return SPP_ERROR_NONE;
}

int spp_tc_build_packet(space_packet_t *space_packet, const uint8_t flag,
                        const uint8_t sec_header, const uint16_t sec_header_len,
                        const uint8_t *data, const uint16_t data_len,
                        spp_apid_context_t *ptr_apid_context) {
  if (space_packet == NULL) {
    return SPP_ERROR_NULL_SPACE_PACKET;
  }
  if (ptr_apid_context == NULL) {
    return SPP_ERROR_NULL_COUNTER;
  }
  spp_validate_counters(ptr_apid_context);
  const int ret = spp_build_packet(space_packet, SPP_PTYPE_TC, flag, sec_header,
                                   sec_header_len, ptr_apid_context->apid,
                                   ptr_apid_context->tc, data, data_len);
  if (ret == SPP_ERROR_NONE) {
    ptr_apid_context->tc++;
  }
  return ret;
}

int spp_tm_build_packet(space_packet_t *space_packet, const uint8_t flag,
                        const uint8_t sec_header, const uint16_t sec_header_len,
                        const uint8_t *data, const uint16_t data_len,
                        spp_apid_context_t *ptr_apid_context) {
  if (space_packet == NULL) {
    return SPP_ERROR_NULL_SPACE_PACKET;
  }
  if (ptr_apid_context == NULL) {
    return SPP_ERROR_NULL_COUNTER;
  }
  spp_validate_counters(ptr_apid_context);
  const int ret = spp_build_packet(space_packet, SPP_PTYPE_TM, flag, sec_header,
                                   sec_header_len, ptr_apid_context->apid,
                                   ptr_apid_context->tm, data, data_len);
  if (ret == SPP_ERROR_NONE) {
    ptr_apid_context->tm++;
  }
  return ret;
}

int spp_idle_build_packet(space_packet_t *space_packet) {
  const uint8_t idle_buffer[SPP_IDLE_BUFFER_LEN] = {SPP_IDLE_BUFFER_DATA};
  return spp_build_packet(space_packet, SPP_PTYPE_TM,
                          SPP_GROUP_FLAG_UNSEGMENTED,
                          SPP_SECHEAD_FLAG_NOPRESENT, 0, SPP_APID_IDLE, 0,
                          idle_buffer, SPP_IDLE_BUFFER_LEN);
}

int spp_unpack_packet(space_packet_t *space_packet, const uint8_t *buffer,
                      const uint16_t buffer_len) {
  if (buffer == NULL) {
    return SPP_ERROR_INVALID_BUFFER;
  }
  if (buffer_len < SPP_PRIMARY_HEADER_LEN) {
    return SPP_ERROR_PACKET_LEN;
  }

  memset(space_packet, 0, sizeof(space_packet_t));

  space_packet->header.identification = ((uint16_t)buffer[0] << 8) | buffer[1];
  space_packet->header.sequence = ((uint16_t)buffer[2] << 8) | buffer[3];
  space_packet->header.length = ((uint16_t)buffer[4] << 8) | buffer[5];

  if (spp_get_version(space_packet) != CCSDS_SPP_VERSION) {
    return SPP_ERROR_VERSION;
  }

  const uint32_t expected_total_len =
      (uint32_t)space_packet->header.length + 1 + SPP_PRIMARY_HEADER_LEN;
  if (buffer_len < expected_total_len) {
    return SPP_ERROR_PACKET_LEN;
  }

  if (space_packet->header.length >= SPP_MAX_PAYLOAD_CHUNK) {
    return SPP_ERROR_PAYLOAD_LEN_OUT_LIMITS;
  }

  memcpy(space_packet->data, buffer + SPP_PRIMARY_HEADER_LEN,
         space_packet->header.length + 1);
  return SPP_ERROR_NONE;
}

int spp_reassemble_segmented_packets(space_packet_t **packets,
                                     uint16_t num_packets,
                                     uint8_t *output_buffer,
                                     uint16_t output_len) {
  if (num_packets == 0 || packets == NULL || output_buffer == NULL) {
    return SPP_ERROR_INVALID_BUFFER;
  }
  uint16_t apid = spp_get_apid(packets[0]);
  uint16_t total_len = 0;
  for (uint16_t i = 0; i < num_packets; i++) {
    if (spp_get_apid(packets[i]) != apid) {
      return SPP_ERROR_INVALID_APID;
    }
    const uint16_t flags = spp_get_sequence_flags(packets[i]);
    if (i == 0 && flags != SPP_GROUP_FLAG_START &&
        flags != SPP_GROUP_FLAG_UNSEGMENTED) {
      return SPP_ERROR_MALFORMED_SEC_HEADER;
    }
    if (i == num_packets - 1 && flags != SPP_GROUP_FLAG_END &&
        flags != SPP_GROUP_FLAG_UNSEGMENTED) {
      return SPP_ERROR_MALFORMED_SEC_HEADER;
    }
    if (total_len + packets[i]->header.length + 1 > output_len) {
      return SPP_ERROR_PAYLOAD_LEN_OUT_LIMITS;
    }
    memcpy(output_buffer + total_len, packets[i]->data,
           packets[i]->header.length + 1);
    total_len += packets[i]->header.length + 1;
  }
  return SPP_ERROR_NONE;
}

uint16_t spp_get_version(const space_packet_t *ptr_packet) {
  return (ptr_packet->header.identification >> 13) & 0x7;
}

uint16_t spp_get_type(const space_packet_t *ptr_packet) {
  return (ptr_packet->header.identification >> 11) & 0x1;
}

uint16_t spp_get_secondary_header(const space_packet_t *ptr_packet) {
  return (ptr_packet->header.identification >> 10) & 0x1;
}

uint16_t spp_get_sequence_flags(const space_packet_t *ptr_packet) {
  return (ptr_packet->header.sequence >> 14) & 0x3;
}

uint16_t spp_get_sequence_count(const space_packet_t *ptr_packet) {
  return ptr_packet->header.sequence & 0x3FFF;
}

uint16_t spp_get_apid(const space_packet_t *ptr_packet) {
  return ptr_packet->header.identification & 0x7FF;
}

int spp_get_secondary_header_data(const space_packet_t *ptr_packet,
                                  uint8_t *sec_header,
                                  uint16_t sec_header_len) {
  if (spp_get_secondary_header(ptr_packet) == SPP_SECHEAD_FLAG_NOPRESENT ||
      sec_header_len == 0) {
    return SPP_ERROR_MALFORMED_SEC_HEADER;
  }
  if (ptr_packet->header.length < sec_header_len - 1) {
    return SPP_ERROR_PAYLOAD_LEN;
  }
  memcpy(sec_header, ptr_packet->data, sec_header_len);
  return SPP_ERROR_NONE;
}
