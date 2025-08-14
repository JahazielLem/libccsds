/*  - main.c
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
/**
 * @brief Reads a file chunk into a fixed-size buffer.
 *
 * @param file_path Path to the file.
 * @param buffer Buffer to store the chunk (size >= CFDP_MAX_PDU_SIZE).
 * @param offset Starting offset in the file.
 * @param bytes_read Pointer to store the number of bytes read.
 * @return 0 on success, negative on error (-1: file not found, -2: seek error,
 * -3: read error).
 */
int read_file_chunk(const char *file_path, uint8_t *buffer, uint32_t offset,
                    uint16_t *bytes_read) {
  FILE *file = fopen(file_path, "rb"); // Open in binary mode
  if (!file) {
    return -1; // File not found
  }

  // Seek to the specified offset
  if (fseek(file, offset, SEEK_SET) != 0) {
    fclose(file);
    return -2; // Seek error
  }

  // Read up to CFDP_MAX_PDU_SIZE bytes
  *bytes_read = fread(buffer, 1, 128, file);
  if (ferror(file)) {
    fclose(file);
    return -3; // Read error
  }

  fclose(file);
  return 0; // Success
}

/**
 * @brief Gets the file size.
 *
 * @param file_path Path to the file.
 * @param file_size Pointer to store the file size.
 * @return 0 on success, negative on error (-1: file not found, -2: seek error).
 */
int get_file_size(const char *file_path, uint16_t *file_size) {
  FILE *file = fopen(file_path, "rb");
  if (!file) {
    return -1; // File not found
  }

  if (fseek(file, 0, SEEK_END) != 0) {
    fclose(file);
    return -2; // Seek error
  }

  *file_size = ftell(file);
  if (*file_size == -1) {
    fclose(file);
    return -2; // Seek error
  }

  fclose(file);
  return 0;
}

int main(int argc, char *argv[]) {
  uint16_t file_size = 0;
  int ret =
      get_file_size("/Users/astrobyte/Repos/ccsds_lib/main/pwnsat", &file_size);
  printf("File size: %d\n", file_size);
  uint16_t offset = 0;
  uint8_t buffer[128];
  uint16_t bytes_read = 0;

  cfdp_send_metadata_pdu();
  while (offset < file_size) {
    if (read_file_chunk("/Users/astrobyte/Repos/ccsds_lib/main/pwnsat", buffer,
                        offset, &bytes_read) != 0) {
      printf("Error reading\n");
      return -2;
    }
    printf("File size: %d - Bytes read: %d - Offset: %d - Remaining: %d\n",
           file_size, bytes_read, offset, (file_size - offset - bytes_read));
    cfdp_send_data_transfer();
    offset += bytes_read;
  }
  cfdp_send_eof_pdu();
  // After received the ACK EOF and Finished PUD packet
  cfdp_send_ack_finished_pdu();
  return 0;
}
