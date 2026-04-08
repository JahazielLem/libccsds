/**
 * @file test/test_spp_invalid.c
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

int test_build_tm_null_data(void) {
  const uint16_t apid = 1;
  space_packet_t packet;
  spp_apid_context_t context = {.tc = 0, .tm = 0, .apid = 1};
  return spp_tm_build_packet(&packet, SPP_GROUP_FLAG_UNSEGMENTED,
                             SPP_SECHEAD_FLAG_NOPRESENT, 0, NULL, 12, &context);
}

int test_build_tc_null_data(void) {
  const uint16_t apid = 1;
  space_packet_t packet;
  spp_apid_context_t context = {.tc = 0, .tm = 0, .apid = 1};
  return spp_tc_build_packet(&packet, SPP_GROUP_FLAG_UNSEGMENTED,
                             SPP_SECHEAD_FLAG_NOPRESENT, 0, NULL, 12, &context);
}

int test_build_tm_overflow_data_len(void) {
  space_packet_t packet;
  spp_apid_context_t context = {.tc = 0, .tm = 0, .apid = 1};
  return spp_tm_build_packet(&packet, SPP_GROUP_FLAG_UNSEGMENTED,
                             SPP_SECHEAD_FLAG_NOPRESENT, 0, buffer,
                             SPP_MAX_PAYLOAD_CHUNK + 2, &context);
}

int test_build_tc_overflow_data_len(void) {
  space_packet_t packet;
  spp_apid_context_t context = {.tc = 0, .tm = 0, .apid = 1};
  return spp_tc_build_packet(&packet, SPP_GROUP_FLAG_UNSEGMENTED,
                             SPP_SECHEAD_FLAG_NOPRESENT, 0, buffer,
                             SPP_MAX_PAYLOAD_CHUNK + 2, &context);
}

int test_build_tm_underflow_data_len(void) {
  space_packet_t packet;
  spp_apid_context_t context = {.tc = 0, .tm = 0, .apid = 1};
  return spp_tm_build_packet(&packet, SPP_GROUP_FLAG_UNSEGMENTED,
                             SPP_SECHEAD_FLAG_NOPRESENT, 0, buffer, 0,
                             &context);
}

int test_build_tc_underflow_data_len(void) {
  space_packet_t packet;
  spp_apid_context_t context = {.tc = 0, .tm = 0, .apid = 1};
  return spp_tc_build_packet(&packet, SPP_GROUP_FLAG_UNSEGMENTED,
                             SPP_SECHEAD_FLAG_NOPRESENT, 0, buffer, 0,
                             &context);
}

int test_build_tm_idle_apid(void) {
  space_packet_t packet;
  spp_apid_context_t context = {.tc = 0, .tm = 0, .apid = SPP_APID_IDLE};
  return spp_tm_build_packet(&packet, SPP_GROUP_FLAG_UNSEGMENTED,
                             SPP_SECHEAD_FLAG_NOPRESENT, 0, buffer, 12,
                             &context);
}

int test_build_tc_idle_apid(void) {
  space_packet_t packet;
  spp_apid_context_t context = {.tc = 0, .tm = 0, .apid = SPP_APID_IDLE};
  return spp_tc_build_packet(&packet, SPP_GROUP_FLAG_UNSEGMENTED,
                             SPP_SECHEAD_FLAG_NOPRESENT, 0, buffer, 12,
                             &context);
}

int test_packet_counter(void) {
  spp_apid_context_t context = {.tc = 16390, .tm = 16390, .apid = 1};
  space_packet_t packet;
  const int ret =
      spp_tc_build_packet(&packet, SPP_GROUP_FLAG_UNSEGMENTED,
                          SPP_SECHEAD_FLAG_NOPRESENT, 0, buffer, 12, &context);

  ASSERT_ERROR(ret == SPP_ERROR_NONE,
               "test_packet_counter.spp_tc_build_packet error.");
  ASSERT_ERROR(context.tm == 0, "test_packet_counter.context.tm error.");
  ASSERT_ERROR(context.tc == 1, "test_packet_counter.context.tc error.");
  return SPP_ERROR_NONE;
}

int test_counter_wrap_exact(void) {
  spp_apid_context_t context = {.tc = 16383, .tm = 0, .apid = 1};
  space_packet_t packet;

  const int ret =
      spp_tc_build_packet(&packet, SPP_GROUP_FLAG_UNSEGMENTED,
                          SPP_SECHEAD_FLAG_NOPRESENT, 0, buffer, 12, &context);

  ASSERT_ERROR(ret == SPP_ERROR_NONE, "wrap failed");
  ASSERT_ERROR(context.tc == 1, "wrap incorrect");

  return SPP_ERROR_NONE;
}

int test_null_counter(void) {
  space_packet_t packet;
  return spp_tm_build_packet(&packet, SPP_GROUP_FLAG_UNSEGMENTED,
                             SPP_SECHEAD_FLAG_NOPRESENT, 0, buffer, 12, NULL);
}

int test_null_packet_ptr(void) {
  spp_apid_context_t context = {.tc = 0, .tm = 0, .apid = 1};
  return spp_tm_build_packet(NULL, SPP_GROUP_FLAG_UNSEGMENTED,
                             SPP_SECHEAD_FLAG_NOPRESENT, 0, buffer, 12,
                             &context);
}

int test_apid_out_of_range(void) {
  space_packet_t packet;
  spp_apid_context_t context = {.tc = 0, .tm = 0, .apid = 0xFFFF};

  return spp_tm_build_packet(&packet, SPP_GROUP_FLAG_UNSEGMENTED,
                             SPP_SECHEAD_FLAG_NOPRESENT, 0, buffer, 12,
                             &context);
}

int test_length_field_correct(void) {
  space_packet_t packet;
  spp_apid_context_t context = {.tc = 0, .tm = 0, .apid = 1};

  spp_tm_build_packet(&packet, SPP_GROUP_FLAG_UNSEGMENTED,
                      SPP_SECHEAD_FLAG_NOPRESENT, 0, buffer, 12, &context);

  const uint16_t len = HOST_TO_BE16(packet.header.length);
  ASSERT_ERROR(len == 11, "length incorrect");
  return SPP_ERROR_NONE;
}

int test_endianness(void) {
  space_packet_t packet;
  spp_apid_context_t context = {.tc = 0, .tm = 0, .apid = 1};

  spp_tm_build_packet(&packet, SPP_GROUP_FLAG_UNSEGMENTED,
                      SPP_SECHEAD_FLAG_NOPRESENT, 0, buffer, 12, &context);

  const uint16_t raw = packet.header.identification;
  ASSERT_ERROR(raw != 1, "endianness incorrect");

  return SPP_ERROR_NONE;
}

int test_sequence_wrap(void) {
  spp_apid_context_t context = {.tc = 16383, .tm = 0, .apid = 1};
  space_packet_t packet;

  const int ret =
      spp_tc_build_packet(&packet, SPP_GROUP_FLAG_UNSEGMENTED,
                          SPP_SECHEAD_FLAG_NOPRESENT, 0, buffer, 12, &context);

  const uint16_t seq = spp_get_sequence_count(&packet);

  ASSERT_ERROR(ret == SPP_ERROR_NONE, "spp_tc_build_packet failed");
  ASSERT_ERROR(seq == 16383, "sequence incorrect before wrap");
  ASSERT_ERROR(context.tc == 0, "context did not wrap");

  return 0;
}

int main(void) {
  ASSERT_ERROR(test_build_tm_null_data() == SPP_ERROR_INVALID_BUFFER,
               "test_build_tm_null_data error.");
  ASSERT_ERROR(test_build_tc_null_data() == SPP_ERROR_INVALID_BUFFER,
               "test_build_tc_null_data error.");

  ASSERT_ERROR(test_build_tm_overflow_data_len() == SPP_ERROR_PAYLOAD_LEN,
               "test_build_tm_overflow_data_len error.");
  ASSERT_ERROR(test_build_tc_overflow_data_len() == SPP_ERROR_PAYLOAD_LEN,
               "test_build_tc_overflow_data_len error.");

  ASSERT_ERROR(test_build_tm_underflow_data_len() == SPP_ERROR_PAYLOAD_LEN,
               "test_build_tm_underflow_data_len error.");
  ASSERT_ERROR(test_build_tc_underflow_data_len() == SPP_ERROR_PAYLOAD_LEN,
               "test_build_tc_underflow_data_len error.");

  ASSERT_ERROR(test_build_tm_idle_apid() == SPP_ERROR_NONE,
               "test_build_tm_idle_apid error.");
  ASSERT_ERROR(test_build_tc_idle_apid() == SPP_ERROR_INVALID_APID,
               "test_build_tc_idle_apid error.");

  ASSERT_ERROR(test_null_packet_ptr() == SPP_ERROR_NULL_SPACE_PACKET,
               "test_null_packet_ptr error.");

  ASSERT_ERROR(test_null_counter() == SPP_ERROR_NULL_COUNTER,
               "test_null_counter error.");
  ASSERT_ERROR(test_counter_wrap_exact() == SPP_ERROR_NONE,
               "test_counter_wrap_exact error.");
  ASSERT_ERROR(test_packet_counter() == SPP_ERROR_NONE,
               "test_packet_counter error.");

  ASSERT_ERROR(test_apid_out_of_range() == SPP_ERROR_INVALID_APID,
               "test_apid_out_of_range error.");

  ASSERT_ERROR(test_length_field_correct() == SPP_ERROR_NONE,
               "test_length_field_correct error.");

  ASSERT_ERROR(test_endianness() == SPP_ERROR_NONE, "test_endianness error.");
  return EXIT_SUCCESS;
}