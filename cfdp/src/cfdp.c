/* cfdp - cfdp.c
 * DESCRIPTION
 *
 * ccsds_lib - LoRa Packet Analyzer
 * By astrobyte 12/08/25.
 *
 * SPDX-License-Identifier: GPL-2.0-or-later
 *
 */
#include "cfdp.h"

#include <stdio.h>

static uint16_t transaction_count = 0;
static uint16_t pdu_count = 0;

int cfdp_build_pdu(cfdp_pdu_header_t *header, const uint8_t type,
                   const uint8_t direction, const uint8_t mode,
                   const uint8_t crc_flag, const uint8_t file_flag,
                   const uint8_t segmentation, const uint8_t entity_id_length,
                   const uint8_t metadata_flag, const uint16_t source_id,
                   const uint16_t destination_id,
                   const uint16_t pdu_data_length,
                   const uint16_t transaction_seq) {
  header->version_type_dir_mode_crc_large =
      (CCSDS_CFDP_VERSION << 5) | (type << 4) | (direction << 3) | (mode << 2) |
      (crc_flag << 1) | file_flag;
  const uint8_t transaction_sequence_length =
      2; // Hardcoded for this implementation -> 2 bytes
  header->reserved_entity_seq_len =
      (segmentation << 7) | (entity_id_length << 4) | (metadata_flag << 3) |
      transaction_sequence_length;
  header->pdu_data_length = pdu_data_length;
  header->source_entity_id = source_id;
  header->dest_entity_id = destination_id;
  header->transaction_seq_num = transaction_seq;
  return 0;
}

void cfdp_send_metadata_pdu() { printf("Sending metadata pdu...\n"); }
void cfdp_send_eof_pdu() { printf("Sending eof pdu...\n"); }
void cfdp_send_ack_eof_pdu() { printf("Sending ack pdu...\n"); }
void cfdp_send_finished_pdu() { printf("Sending finished pdu...\n"); }
void cfdp_send_ack_finished_pdu() { printf("Sending ack finished pdu...\n"); }
void cfdp_send_data_transfer() { printf("Sending data transfer pdu...\n"); }
void cfdp_send_nack() { printf("Sending nack pdu...\n"); }