/**
 * @file spp.h
 * @brief Space Packet Protocol (SPP) Library Header for CCSDS-compliant packet
 * handling
 *
 * This header defines the data structures, constants, and function prototypes
 * for the Pwnsat Space Packet Protocol (SPP) library, compliant with CCSDS
 * 133.0-B-2. It supports building and parsing telemetry (TM) and telecommand
 * (TC) packets, managing sequence counters, and handling idle packets for
 * satellite communication.
 *
 * @author Pwnsat Team
 * @date 2025-08-07
 * @license GNU General Public License
 * @copyright Copyright (c) 2025 Pwnsat
 * @contact <contact email or project website>
 */
#ifdef __cplusplus
extern "C" {
#endif
#ifndef __SPP_H
#define __SPP_H

#include "spp_config.h"
#include <stdint.h>
#include <string.h>

/**
 * @brief CCSDS Space Packet Protocol version number (3 bits).
 *
 * Defines the version of the CCSDS SPP as per 133.0-B-2, set to 0.
 */
#define CCSDS_SPP_VERSION 0

/**
 * @brief APID for idle packets.
 *
 * Identifies idle packets with the maximum 11-bit APID value (0x7FF).
 */
#define SPP_APID_IDLE 0b1111111111

/**
 * @brief Length of the CCSDS primary header in bytes.
 *
 * The primary header is 6 bytes, as defined in CCSDS 133.0-B-2.
 */
#define SPP_PRIMARY_HEADER_LEN (6)

/**
 * @brief Error codes for SPP operations.
 *
 * Enumeration of possible error codes returned by SPP functions.
 */
enum {
  SPP_ERROR_NONE = 0,         /**< @brief No error occurred */
  SPP_ERROR_PAYLOAD_LEN = -1, /**< @brief Invalid payload length */
  SPP_ERROR_PACKET_LEN = -2,  /**< @brief Packet length too short */
  SPP_ERROR_PAYLOAD_LEN_OUT_LIMITS =
      -3,                 /**< @brief Payload exceeds maximum size */
  SPP_ERROR_VERSION = -4, /**< @brief Invalid CCSDS version */
  SPP_ERROR_MALFORMED_SEC_HEADER = -5, /**< @brief Malformed secondary header */
  SPP_ERROR_INVALID_BUFFER = -6,       /**< @brief Invalid input buffer */
  SPP_ERROR_INVALID_APID = -7          /**< @brief Invalid APID value */
};

/**
 * @brief Packet type identifiers.
 *
 * Defines the type field (1 bit) in the CCSDS packet identification.
 */
enum {
  SPP_PTYPE_TM = 0x00, /**< @brief Telemetry or Reporting packet */
  SPP_PTYPE_TC = 0x01  /**< @brief Telecommand or Requesting packet */
};

/**
 * @brief Secondary header flag values.
 *
 * Indicates whether a secondary header is present in the packet.
 */
enum {
  SPP_SECHEAD_FLAG_NOPRESENT =
      0x00, /**< @brief No secondary header (e.g., idle packets) */
  SPP_SECHEAD_FLAG_PRESENT = 0x01 /**< @brief Secondary header present */
};

/**
 * @brief Sequence flag values for packet segmentation.
 *
 * Defines the segmentation status of the packet's user data.
 */
enum {
  SPP_GROUP_FLAG_CONT = 0b00,  /**< @brief Continuation segment of user data */
  SPP_GROUP_FLAG_START = 0b01, /**< @brief First segment of user data */
  SPP_GROUP_FLAG_END = 0b10,   /**< @brief Last segment of user data */
  SPP_GROUP_FLAG_UNSEGMENTED = 0b11 /**< @brief Unsegmented user data */
};

/**
 * @brief CCSDS primary header structure (6 bytes).
 *
 * Represents the 6-byte primary header as defined in CCSDS 133.0-B-2,
 * containing packet version, identification, sequence control, and data length
 * fields.
 */
typedef struct {
  uint16_t identification; /**< @brief Version (3 bits), Type (1 bit), SecHdr (1
                              bit), APID (11 bits) */
  uint16_t
      sequence; /**< @brief Sequence flags (2 bits), Sequence count (14 bits) */
  uint16_t
      length; /**< @brief Packet length = (payload + secondary header) - 1 */
} __attribute__((packed)) sp_primary_header_t;

/**
 * @brief CCSDS Space Packet structure.
 *
 * Combines the primary header and payload data for a complete CCSDS packet.
 */
typedef struct {
  sp_primary_header_t header;          /**< @brief Primary header */
  uint8_t data[SPP_MAX_PAYLOAD_CHUNK]; /**< @brief Secondary header + payload
                                          data */
} __attribute__((packed)) space_packet_t;

/**
 * @brief Builds a telecommand (TC) packet.
 *
 * Constructs a CCSDS telecommand packet, increments the TC sequence counter,
 * and validates input parameters.
 *
 * @param space_packet Pointer to the packet structure to fill.
 * @param flag Sequence flag (e.g., SPP_GROUP_FLAG_UNSEGMENTED).
 * @param sec_header Secondary header flag (e.g., SPP_SECHEAD_FLAG_NOPRESENT).
 * @param sec_header_len Length of the secondary header in bytes.
 * @param apid Application Process ID (11 bits).
 * @param data Pointer to the payload data.
 * @param data_len Length of the payload data in bytes.
 * @return SPP_ERROR_NONE on success, or an error code on failure.
 */
int spp_tc_build_packet(space_packet_t *space_packet, uint8_t flag,
                        uint8_t sec_header, uint16_t sec_header_len,
                        uint16_t apid, const uint8_t *data, uint16_t data_len);

/**
 * @brief Builds a telemetry (TM) packet.
 *
 * Constructs a CCSDS telemetry packet, increments the TM sequence counter, and
 * validates input parameters.
 *
 * @param space_packet Pointer to the packet structure to fill.
 * @param flag Sequence flag (e.g., SPP_GROUP_FLAG_UNSEGMENTED).
 * @param sec_header Secondary header flag (e.g., SPP_SECHEAD_FLAG_NOPRESENT).
 * @param sec_header_len Length of the secondary header in bytes.
 * @param apid Application Process ID (11 bits).
 * @param data Pointer to the payload data.
 * @param data_len Length of the payload data in bytes.
 * @return SPP_ERROR_NONE on success, or an error code on failure.
 */
int spp_tm_build_packet(space_packet_t *space_packet, uint8_t flag,
                        uint8_t sec_header, uint16_t sec_header_len,
                        uint16_t apid, const uint8_t *data, uint16_t data_len);

/**
 * @brief Builds an idle packet.
 *
 * Constructs a CCSDS idle packet with APID = SPP_APID_IDLE and a fixed payload
 * containing "HackTheWorld".
 *
 * @param space_packet Pointer to the packet structure to fill.
 * @return SPP_ERROR_NONE on success, or an error code on failure.
 */
int spp_idle_build_packet(space_packet_t *space_packet);

/**
 * @brief Unpacks a CCSDS packet from a raw buffer.
 *
 * Parses a raw byte buffer into a space_packet_t structure, validating the
 * packet length and version.
 *
 * @param space_packet Pointer to the packet structure to fill.
 * @param buffer Pointer to the raw packet data.
 * @param buffer_len Length of the raw packet data in bytes.
 * @return SPP_ERROR_NONE on success, or an error code on failure.
 */
int spp_unpack_packet(space_packet_t *space_packet, const uint8_t *buffer,
                      uint16_t buffer_len);

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
                                     uint16_t output_len);

/**
 * @brief Prints detailed information about a CCSDS packet.
 *
 * Outputs packet details (e.g., version, APID, sequence count) to the console
 * for debugging purposes.
 *
 * @param packet Pointer to the packet structure to print.
 */
void spp_print_packet_details(space_packet_t *packet);

/**
 * @brief Gets the packet version number.
 *
 * Extracts the 3-bit version field from the packet's identification field.
 *
 * @param packet Pointer to the packet structure.
 * @return The 3-bit version number.
 */
uint16_t spp_get_version(space_packet_t *packet);

/**
 * @brief Gets the packet type.
 *
 * Extracts the 1-bit type field (TM or TC) from the packet's identification
 * field.
 *
 * @param packet Pointer to the packet structure.
 * @return The 1-bit type (SPP_PTYPE_TM or SPP_PTYPE_TC).
 */
uint16_t spp_get_type(space_packet_t *packet);

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
uint16_t spp_get_secondary_header(space_packet_t *packet);

/**
 * @brief Gets the sequence flags.
 *
 * Extracts the 2-bit sequence flags from the packet's sequence field.
 *
 * @param packet Pointer to the packet structure.
 * @return The 2-bit sequence flags (e.g., SPP_GROUP_FLAG_UNSEGMENTED).
 */
uint16_t spp_get_sequence_flags(space_packet_t *packet);

/**
 * @brief Gets the sequence count.
 *
 * Extracts the 14-bit sequence count from the packet's sequence field.
 *
 * @param packet Pointer to the packet structure.
 * @return The 14-bit sequence count.
 */
uint16_t spp_get_sequence_count(space_packet_t *packet);

/**
 * @brief Gets the Application Process ID (APID).
 *
 * Extracts the 11-bit APID from the packet's identification field.
 *
 * @param packet Pointer to the packet structure.
 * @return The 11-bit APID.
 */
uint16_t spp_get_apid(space_packet_t *packet);

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
                                  uint16_t sec_header_len);

#endif
#ifdef __cplusplus
}
#endif