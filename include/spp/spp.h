/**
 * @file spp.h
 * @brief Space Packet Protocol (SPP) core definitions.
 * * This module implements the CCSDS 133.0-B-2 standard for Space Packets,
 * providing the necessary structures and constants for TM/TC handling.
 * * @author astrobyte
 * @date 2026-04-08
 * @license GNU General Public License
 * @copyright Copyright (c) 2026 Kevin Leon
 */

#ifndef LIBCCSDS_SPP_H
#define LIBCCSDS_SPP_H

#include <spp_config.h>
#include <stdint.h>

/** @brief CCSDS Space Packet Protocol version (3 bits). Always 0 for Version 1.
 */
#define CCSDS_SPP_VERSION 0

/** @brief APID reserved for IDLE packets. */
#define SPP_APID_IDLE 0x7FF

/** @brief Fixed length of the CCSDS Primary Header in bytes. */
#define SPP_PRIMARY_HEADER_LEN (6)

/** @brief Default length for internal IDLE buffer. */
#define SPP_IDLE_BUFFER_LEN (1)
/** @brief Default data pattern for IDLE packets. */
#define SPP_IDLE_BUFFER_DATA 0x0

/**
 * @name Packet Type Identifiers
 * @{ */
enum {
  SPP_PTYPE_TM = 0x00, /**< Telemetry or Reporting packet. */
  SPP_PTYPE_TC = 0x01  /**< Telecommand or Requesting packet. */
};
/** @} */

/**
 * @name Secondary Header Flags
 * @{ */
enum {
  SPP_SECHEAD_FLAG_NOPRESENT = 0x00, /**< No secondary header present. */
  SPP_SECHEAD_FLAG_PRESENT =
      0x01 /**< Secondary header follows primary header. */
};
/** @} */

/**
 * @name Sequence Group Flags
 * @{ */
enum {
  SPP_GROUP_FLAG_CONT = 0x0,       /**< Continuation segment of user data. */
  SPP_GROUP_FLAG_START = 0x1,      /**< First segment of user data. */
  SPP_GROUP_FLAG_END = 0x2,        /**< Last segment of user data. */
  SPP_GROUP_FLAG_UNSEGMENTED = 0x3 /**< Complete, unsegmented user data. */
};
/** @} */

/**
 * @brief Context structure for managing APID-specific sequence counters.
 */
typedef struct {
  uint16_t tc;   /**< Telecommand sequence counter (14-bit wrap-around). */
  uint16_t tm;   /**< Telemetry sequence counter (14-bit wrap-around). */
  uint16_t apid; /**< Application Process Identifier (11 bits). */
} spp_apid_context_t;

/**
 * @brief CCSDS Primary Header structure (6 bytes).
 * * @note This structure is marked as packed to ensure exact memory mapping
 * for hardware-level communication.
 */
typedef struct {
  uint16_t identification; /**< Version (3b), Type (1b), Sec. Header Flag (1b),
                              APID (11b). */
  uint16_t sequence;       /**< Sequence Flags (2b), Sequence Count (14b). */
  uint16_t length;         /**< Total payload length - 1 (16 bits). */
} __attribute__((packed)) sp_primary_header_t;

/**
 * @brief Complete CCSDS Space Packet structure.
 */
typedef struct {
  sp_primary_header_t header;          /**< 6-byte Primary Header. */
  uint8_t data[SPP_MAX_PAYLOAD_CHUNK]; /**< Payload data (includes Secondary
                                          Header if present). */
} __attribute__((packed)) space_packet_t;

/* Function Prototypes */

/**
 * @brief Constructs a Telecommand (TC) packet.
 * * @param[out] space_packet Destination packet structure.
 * @param[in] flag Sequence group flags.
 * @param[in] sec_header Secondary header presence flag.
 * @param[in] sec_header_len Length of the secondary header in bytes.
 * @param[in] data Pointer to user payload data.
 * @param[in] data_len Length of the user payload.
 * @param[in,out] ptr_apid_context Pointer to the APID context to update
 * counters.
 * @return SPP_ERROR_NONE on success, or specific error code.
 */
int spp_tc_build_packet(space_packet_t *space_packet, uint8_t flag,
                        uint8_t sec_header, uint16_t sec_header_len,
                        const uint8_t *data, uint16_t data_len,
                        spp_apid_context_t *ptr_apid_context);

/**
 * @brief Constructs a Telemetry (TM) packet.
 */
int spp_tm_build_packet(space_packet_t *space_packet, uint8_t flag,
                        uint8_t sec_header, uint16_t sec_header_len,
                        const uint8_t *data, uint16_t data_len,
                        spp_apid_context_t *ptr_apid_context);

/**
 * @brief Builds a standard CCSDS IDLE packet.
 * @return SPP_ERROR_NONE on success.
 */
int spp_idle_build_packet(space_packet_t *space_packet);

/**
 * @brief Unpacks a raw byte buffer into a space_packet_t structure.
 * * Performs safety checks on buffer length, version, and payload limits.
 * * @param[out] space_packet Structure to populate.
 * @param[in] buffer Raw input data.
 * @param[in] buffer_len Size of the input buffer.
 * @return SPP_ERROR_NONE or error code.
 */
int spp_unpack_packet(space_packet_t *space_packet, const uint8_t *buffer,
                      uint16_t buffer_len);

/**
 * @brief Reassembles segmented packets into a single contiguous buffer.
 */
int spp_reassemble_segmented_packets(space_packet_t **packets,
                                     uint16_t num_packets,
                                     uint8_t *output_buffer,
                                     uint16_t output_len);

/* Header Field Getters */
uint16_t spp_get_version(const space_packet_t *ptr_packet);
uint16_t spp_get_type(const space_packet_t *ptr_packet);
uint16_t spp_get_secondary_header(const space_packet_t *ptr_packet);
uint16_t spp_get_sequence_flags(const space_packet_t *ptr_packet);
uint16_t spp_get_sequence_count(const space_packet_t *ptr_packet);
uint16_t spp_get_apid(const space_packet_t *ptr_packet);

/**
 * @brief Extracts secondary header data from a packet.
 */
int spp_get_secondary_header_data(const space_packet_t *ptr_packet,
                                  uint8_t *sec_header, uint16_t sec_header_len);

#endif // LIBCCSDS_SPP_H