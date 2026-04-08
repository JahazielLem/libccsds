/**
 * @file test/test_spp_unpack.c
 * @brief PROJECT
 *
 * DESCRIPTION
 *
 * @author astrobyte
 * @date 2026-04-08
 * @license GNU General Public License
 * @copyright Copyright (c) 2026 kevin Leon
 * @contact kevinleon.morales@gmail.com
 */

#include <ccsds.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>

#define ASSERT_ERROR(cond, msg)                                                \
  if (!(cond)) {                                                               \
    printf("FAIL: %s\n", msg);                                                 \
    return 1;                                                                  \
  }

int test_unpack_security(void) {
  space_packet_t packet;
  // Malformed buffer
  const uint8_t corrupt_buffer[6] = {0x00, 0x01, 0xC0, 0x01, 0x00, 0xC8};

  return spp_unpack_packet(&packet, corrupt_buffer, 6);
}

int main(void) {
  ASSERT_ERROR(test_unpack_security() == SPP_ERROR_PACKET_LEN, "Unpack error.");
  return EXIT_SUCCESS;
}