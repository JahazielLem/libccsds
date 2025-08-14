/**
 * @file cfdp.h
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
#ifndef __CFDP_H
#define __CFDP_H

#include <stdint.h>
#include <string.h>

#define CCSDS_CFDP_VERSION 0x1
#define CFDP_MAX_PAYLOAD_CHUNK 128 // Normal operational value 248

enum { CFDP_PDU_DIRECTIVE = 0x00, CFDP_PDU_DATA = 0x01 };

enum { CFDP_DIR_RECEIVER = 0x00, CFDP_DIR_SENDER = 0x01 };

enum { CFDP_MODE_ACK = 0x00, CFDP_MODE_NACK = 0x01 };

enum { CFDP_CRC_NONE = 0x00, CFDP_CRC_PRESENT = 0x01 };

enum { CFDP_FILE_FLAG_SMALL = 0x00, CFDP_FILE_FLAG_LARGE = 0x01 };

enum {
  CFDP_SEGMENTATION_CONTROL_NONE = 0x00,
  CFDP_SEGMENTATION_CONTROL_PRESERVED = 0x01
};

enum { CFDP_METADATA_FLAG_NONE = 0x00, CFDP_METADATA_FLAG_PRESENT = 0x01 };

enum {
  CFDP_DIRECTIVE_EOF_PDU = 0x04,
  CFDP_DIRECTIVE_FINISHED_PDU = 0x05,
  CFDP_DIRECTIVE_ACK_PDU = 0x06,
  CFDP_DIRECTIVE_METADATA_PDU = 0x07,
  CFDP_DIRECTIVE_NACK_PDU = 0x08,
  CFDP_DIRECTIVE_PROMPT_PDU = 0x08,
  CFDP_DIRECTIVE_KEEP_ALIVE_PDU = 0x08,
};

// We use uin16_t instead of the common uint32_t by the kind of parameters of
// pwnsat
typedef struct {
  uint8_t
      version_type_dir_mode_crc_large; // Bits: Version (3), Type (1), Direction
                                       // (1), Mode (1), CRC (1), Large File (1)
  uint8_t reserved_entity_seq_len; // Bits: Segmentation control (1), Entity ID
                                   // Length (3), Segment metadata (1), Seq Num
                                   // Length (3)
  uint16_t
      pdu_data_length; // Length of PDU data field (excluding header and CRC)
  uint16_t source_entity_id; // 2-byte ID for source (e.g., spacecraft, 0x01)
  uint16_t
      dest_entity_id; // 2-byte ID for destination (e.g., ground station, 0x02)
  uint16_t transaction_seq_num; // 2-byte transaction ID
} __attribute__((packed)) cfdp_pdu_header_t;

typedef struct {
  cfdp_pdu_header_t header;
  uint8_t directive_code; // For File Directive PDUs
  uint8_t data[CFDP_MAX_PAYLOAD_CHUNK];
  uint16_t crc;
} __attribute__((packed)) cfdp_pdu_t;

void cfdp_send_metadata_pdu();
void cfdp_send_eof_pdu();
void cfdp_send_ack_eof_pdu();
void cfdp_send_finished_pdu();
void cfdp_send_ack_finished_pdu();
void cfdp_send_data_transfer();
void cfdp_send_nack();

#endif
#ifdef __cplusplus
}
#endif