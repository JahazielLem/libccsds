/**
 * @file spp.c
 * @brief Space Packet Protocol (SPP) Library Implementation for CCSDS-compliant
 * packet handling
 *
 * This file implements the Pwnsat Space Packet Protocol (SPP) library,
 * compliant with CCSDS 133.0-B-2. It provides functions for building and
 * parsing telemetry (TM) and telecommand (TC) packets, managing sequence
 * counters, and handling idle packets for satellite communication.
 *
 * @author Pwnsat Team
 * @date 2025-08-07
 * @license GNU General Public License
 * @copyright Copyright (c) 2025 Pwnsat
 * @contact <contact email or project website>
 */
#include <spp.h>

/**
 * @brief Structure to track packet sequence counters for TM and TC packets.
 */
typedef struct {
  uint16_t tc; /**< @brief Telecommand packet sequence counter (14 bits, max
                  16383) */
  uint16_t
      tm; /**< @brief Telemetry packet sequence counter (14 bits, max 16383) */
} packet_counter_t;

/**
 * @brief Static packet counter instance.
 *
 * Maintains separate sequence counters for telemetry and telecommand packets.
 */
static packet_counter_t packet_counter = {.tc = 0, .tm = 0};

/**
 * @brief IDLE buffer len.
 *
 * Static const for IDLE Packages. (This is pwnsat implementation).
 */
static const uint16_t buffer_len = 13;

/**
 * @brief IDLE buffer.
 *
 * HackTheWorld IDLE package. (This is pwnsat implementation).
 */
static const uint8_t buffer_idle[buffer_len] = {0x48, 0x61, 0x63, 0x6b, 0x54,
                                                0x68, 0x65, 0x57, 0x6f, 0x72,
                                                0x6c, 0x64, 0x00};

/**
 * @brief Validates and resets sequence counters.
 *
 * Resets the telecommand (TC) or telemetry (TM) counter to 0 if it reaches the
 * maximum 14-bit value (16383) to prevent overflow.
 */
static void spp_validate_counters() {
  if (packet_counter.tc == 16383) {
    packet_counter.tc = 0;
  }
  if (packet_counter.tm == 16383) {
    packet_counter.tm = 0;
  }
}

/**
 * @brief Builds a CCSDS Space Packet.
 *
 * Constructs a CCSDS packet with the specified type, flags, APID, and payload.
 * Validates input parameters and sets the primary header fields according to
 * CCSDS 133.0-B-2.
 *
 * @param space_packet Pointer to the packet structure to fill.
 * @param type Packet type (SPP_PTYPE_TM or SPP_PTYPE_TC).
 * @param flag Sequence flag (e.g., SPP_GROUP_FLAG_UNSEGMENTED).
 * @param sec_header Secondary header flag (SPP_SECHEAD_FLAG_NOPRESENT or
 * SPP_SECHEAD_FLAG_PRESENT).
 * @param sec_header_len Length of the secondary header in bytes.
 * @param apid Application Process ID (11 bits, 0–2047).
 * @param sequence_count Sequence count for the packet (14 bits, 0–16383).
 * @param data Pointer to the payload data.
 * @param data_len Length of the payload data in bytes.
 * @return SPP_ERROR_NONE on success, or an error code (e.g.,
 * SPP_ERROR_PAYLOAD_LEN) on failure.
 */
static int spp_build_packet(space_packet_t *space_packet, uint8_t type,
                            uint8_t flag, uint8_t sec_header,
                            uint16_t sec_header_len, uint16_t apid,
                            uint16_t sequence_count, const uint8_t *data,
                            uint16_t data_len) {
  if (data_len > SPP_MAX_PAYLOAD_CHUNK) {
    return SPP_ERROR_PAYLOAD_LEN_OUT_LIMITS;
  }
  if (apid == SPP_APID_IDLE && type != SPP_PTYPE_TM) {
    return SPP_ERROR_INVALID_APID;
  }

  memset(space_packet, 0, sizeof(space_packet_t));

  space_packet->header.identification = (CCSDS_SPP_VERSION << 13) |
                                        (type << 11) | (sec_header << 10) |
                                        (apid & 0x7FF);
  space_packet->header.sequence = (flag << 14) | (sequence_count & 0x3FFF);

  if (sec_header == SPP_SECHEAD_FLAG_PRESENT && sec_header_len == 0) {
    return SPP_ERROR_MALFORMED_SEC_HEADER;
  }

  space_packet->header.length = data_len + sec_header_len - 1;

  if (data_len > 0 && data != NULL) {
    memcpy(space_packet->data, data, data_len);
  }

  return SPP_ERROR_NONE;
}

/**
 * @brief Builds a CCSDS telecommand (TC) packet.
 *
 * Constructs a telecommand packet, increments the TC sequence counter, and
 * calls spp_build_packet with the appropriate parameters.
 *
 * @param space_packet Pointer to the packet structure to fill.
 * @param flag Sequence flag (e.g., SPP_GROUP_FLAG_UNSEGMENTED).
 * @param sec_header Secondary header flag (SPP_SECHEAD_FLAG_NOPRESENT or
 * SPP_SECHEAD_FLAG_PRESENT).
 * @param sec_header_len Length of the secondary header in bytes.
 * @param apid Application Process ID (11 bits).
 * @param data Pointer to the payload data.
 * @param data_len Length of the payload data in bytes.
 * @return SPP_ERROR_NONE on success, or an error code on failure.
 */
int spp_tc_build_packet(space_packet_t *space_packet, uint8_t flag,
                        uint8_t sec_header, uint16_t sec_header_len,
                        uint16_t apid, const uint8_t *data, uint16_t data_len) {
  spp_validate_counters();
  packet_counter.tc++;
  return spp_build_packet(space_packet, SPP_PTYPE_TC, flag, sec_header,
                          sec_header_len, apid, packet_counter.tc, data,
                          data_len);
}

/**
 * @brief Builds a CCSDS telemetry (TM) packet.
 *
 * Constructs a telemetry packet, increments the TM sequence counter, and calls
 * spp_build_packet with the appropriate parameters.
 *
 * @param space_packet Pointer to the packet structure to fill.
 * @param flag Sequence flag (e.g., SPP_GROUP_FLAG_UNSEGMENTED).
 * @param sec_header Secondary header flag (SPP_SECHEAD_FLAG_NOPRESENT or
 * SPP_SECHEAD_FLAG_PRESENT).
 * @param sec_header_len Length of the secondary header in bytes.
 * @param apid Application Process ID (11 bits).
 * @param data Pointer to the payload data.
 * @param data_len Length of the payload data in bytes.
 * @return SPP_ERROR_NONE on success, or an error code on failure.
 */
int spp_tm_build_packet(space_packet_t *space_packet, uint8_t flag,
                        uint8_t sec_header, uint16_t sec_header_len,
                        uint16_t apid, const uint8_t *data, uint16_t data_len) {
  spp_validate_counters();
  packet_counter.tm++;
  return spp_build_packet(space_packet, SPP_PTYPE_TM, flag, sec_header,
                          sec_header_len, apid, packet_counter.tm, data,
                          data_len);
}

/**
 * @brief Builds a CCSDS idle packet.
 *
 * Constructs an idle telemetry packet with APID = SPP_APID_IDLE (0x7FF) and a
 * fixed 13-byte payload containing the string "HackTheWorld" followed by a null
 * byte.
 *
 * @param space_packet Pointer to the packet structure to fill.
 * @return SPP_ERROR_NONE on success, or an error code on failure.
 */
int spp_idle_build_packet(space_packet_t *space_packet) {
  return spp_build_packet(
      space_packet, SPP_PTYPE_TM, SPP_GROUP_FLAG_UNSEGMENTED,
      SPP_SECHEAD_FLAG_NOPRESENT, 0, SPP_APID_IDLE, 0, buffer_idle, buffer_len);
}

/**
 * @brief Unpacks a CCSDS packet from a raw buffer.
 *
 * Parses a raw byte buffer into a space_packet_t structure, validating the
 * packet length, version, and payload size. Extracts the primary header and
 * payload data.
 *
 * @param space_packet Pointer to the packet structure to fill.
 * @param buffer Pointer to the raw packet data.
 * @param buffer_len Length of the raw packet data in bytes.
 * @return SPP_ERROR_NONE on success, or an error code (e.g.,
 * SPP_ERROR_PACKET_LEN) on failure.
 */
int spp_unpack_packet(space_packet_t *space_packet, const uint8_t *buffer,
                      uint16_t buffer_len) {
  if (buffer_len < SPP_PRIMARY_HEADER_LEN) {
    return SPP_ERROR_PACKET_LEN;
  }
  if (buffer == NULL) {
    return SPP_ERROR_INVALID_BUFFER;
  }

  memset(space_packet, 0, sizeof(space_packet_t));
  space_packet->header.identification = (buffer[1] << 8) | buffer[0];

  uint8_t version = (space_packet->header.identification >> 13) & 0x07;
  if (version != CCSDS_SPP_VERSION) {
    return SPP_ERROR_VERSION;
  }

  space_packet->header.sequence = (buffer[3] << 8) | buffer[2];
  space_packet->header.length = (buffer[5] << 8) | buffer[4];

  if (space_packet->header.length > SPP_MAX_PAYLOAD_CHUNK) {
    return SPP_ERROR_PAYLOAD_LEN_OUT_LIMITS;
  }

  if (space_packet->header.length > 0 && buffer != NULL) {
    memcpy(space_packet->data, buffer + SPP_PRIMARY_HEADER_LEN,
           space_packet->header.length + 1);
  }
  return SPP_ERROR_NONE;
}

/**
 * @brief Reassembles segmented packets into a single buffer.
 *
 * Combines packets with matching APIDs and sequence flags (START, CONT, END)
 * into a single data buffer.
 *
 * @param packets Array of pointers to segmented packets.
 * @param num_packets Number of packets to reassemble.
 * @param output_buffer Buffer to store the reassembled data.
 * @param output_len Maximum length of the output buffer.
 * @return SPP_ERROR_NONE on success, or an error code on failure.
 */
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
    uint16_t flags = spp_get_sequence_flags(packets[i]);
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

/**
 * @brief Gets the packet version number.
 *
 * Extracts the 3-bit version field from the packet's identification field.
 *
 * @param packet Pointer to the packet structure.
 * @return The 3-bit version number (0–7).
 */
uint16_t spp_get_version(space_packet_t *packet) {
  return (packet->header.identification >> 13) & 0x7;
}

/**
 * @brief Gets the packet type.
 *
 * Extracts the 1-bit type field from the packet's identification field.
 *
 * @param packet Pointer to the packet structure.
 * @return The 1-bit type (SPP_PTYPE_TM or SPP_PTYPE_TC).
 */
uint16_t spp_get_type(space_packet_t *packet) {
  return (packet->header.identification >> 11) & 0x1;
}

/**
 * @brief Gets the secondary header flag.
 *
 * Extracts the 1-bit secondary header flag from the packet's identification
 * field.
 *
 * @param packet Pointer to the packet structure.
 * @return The 1-bit secondary header flag (SPP_SECHEAD_FLAG_NOPRESENT or
 * SPP_SECHEAD_FLAG_PRESENT).
 */
uint16_t spp_get_secondary_header(space_packet_t *packet) {
  return (packet->header.identification >> 10) & 0x1;
}

/**
 * @brief Gets the sequence flags.
 *
 * Extracts the 2-bit sequence flags from the packet's sequence field.
 *
 * @param packet Pointer to the packet structure.
 * @return The 2-bit sequence flags (e.g., SPP_GROUP_FLAG_UNSEGMENTED).
 */
uint16_t spp_get_sequence_flags(space_packet_t *packet) {
  return (packet->header.sequence >> 14) & 0x3;
}

/**
 * @brief Gets the sequence count.
 *
 * Extracts the 14-bit sequence count from the packet's sequence field.
 *
 * @param packet Pointer to the packet structure.
 * @return The 14-bit sequence count (0–16383).
 */
uint16_t spp_get_sequence_count(space_packet_t *packet) {
  return packet->header.sequence & 0x3FFF;
}

/**
 * @brief Gets the Application Process ID (APID).
 *
 * Extracts the 11-bit APID from the packet's identification field.
 *
 * @param packet Pointer to the packet structure.
 * @return The 11-bit APID (0–2047).
 */
uint16_t spp_get_apid(space_packet_t *packet) {
  return packet->header.identification & 0x7FF;
}

/**
 * @brief Extracts the secondary header from a packet.
 *
 * Copies the secondary header data from the packet’s data field into a
 * user-provided buffer.
 *
 * @param packet Pointer to the packet structure.
 * @param sec_header Buffer to store the secondary header data.
 * @param sec_header_len Length of the secondary header to extract.
 * @return SPP_ERROR_NONE on success, or an error code (e.g.,
 * SPP_ERROR_MALFORMED_SEC_HEADER) on failure.
 */
int spp_get_secondary_header_data(space_packet_t *packet, uint8_t *sec_header,
                                  uint16_t sec_header_len) {
  if (spp_get_secondary_header(packet) == SPP_SECHEAD_FLAG_NOPRESENT ||
      sec_header_len == 0) {
    return SPP_ERROR_MALFORMED_SEC_HEADER;
  }
  if (packet->header.length < sec_header_len - 1) {
    return SPP_ERROR_PAYLOAD_LEN;
  }
  memcpy(sec_header, packet->data, sec_header_len);
  return SPP_ERROR_NONE;
}