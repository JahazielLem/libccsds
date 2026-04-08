/**
 * @file test/test_spp_build.c
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

static const uint8_t buffer[12] = {0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20,
                                   0x57, 0x6f, 0x72, 0x6c, 0x64, 0x00};

int test_spp_tm_build_packet(void) {
  const uint16_t apid = 1;
  space_packet_t packet;

  spp_apid_context_t counter = {.tc = 0, .tm = 0, .apid = 1};
  return spp_tm_build_packet(&packet, SPP_GROUP_FLAG_UNSEGMENTED,
                             SPP_SECHEAD_FLAG_NOPRESENT, 0, buffer,
                             sizeof(buffer), &counter);
}

int test_spp_tc_build_packet(void) {
  const uint16_t apid = 1;
  space_packet_t packet;

  spp_apid_context_t counter = {.tc = 0, .tm = 0, .apid = 1};
  return spp_tc_build_packet(&packet, SPP_GROUP_FLAG_UNSEGMENTED,
                             SPP_SECHEAD_FLAG_NOPRESENT, 0, buffer,
                             sizeof(buffer), &counter);
}

int main(void) {
  ASSERT_ERROR(test_spp_tm_build_packet() == SPP_ERROR_NONE,
               "spp_tm_build_packet failed");
  ASSERT_ERROR(test_spp_tc_build_packet() == SPP_ERROR_NONE,
               "spp_tc_build_packet failed");
  return EXIT_SUCCESS;
}