/**
 * @file examples/example_spp.c
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

#include "spp/spp.h"
#include <stdio.h>

int main(void) {
  const uint8_t buffer[12] = {0x48, 0x65, 0x6c, 0x6c, 0x6f, 0x20,
                              0x57, 0x6f, 0x72, 0x6c, 0x64, 0x00};
  space_packet_t packet;
  spp_apid_context_t counter = {.tc = 0, .tm = 0, .apid = 1};
  const int ret =
      spp_tm_build_packet(&packet, SPP_GROUP_FLAG_UNSEGMENTED,
                          SPP_SECHEAD_FLAG_NOPRESENT, 0, buffer, 12, &counter);
  if (ret != SPP_ERROR_NONE) {
    printf("[ERROR] SPP: %d\n", ret);
    return ret;
  }

  printf("[INFO] SPP OK\n");
  return 0;
}
