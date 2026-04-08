/**
 * @file common.h
 * @brief BRIEF
 *
 * DESCRIPTION
 *
 * @author Kevin
 * @date 2026-04-07
 * @license GNU General Public License
 * @copyright Copyright (c) 2026 kevin Leon
 * @contact kevinleon.morales@gmail.com
 */
#ifndef LIBCCSDS_COMMON_H
#define LIBCCSDS_COMMON_H

#include <stdint.h>

#define HOST_TO_BE16(n)                                                        \
  (uint16_t)((((uint16_t)(n) & 0xFF00) >> 8) | (((uint16_t)(n) & 0x00FF) << 8))

/**
 * @brief Error codes for SPP operations.
 *
 * Enumeration of possible error codes returned by SPP functions.
 */

#define SPP_ERROR_NONE 0         /**< @brief No error occurred */
#define SPP_ERROR_PAYLOAD_LEN -1 /**< @brief Invalid payload length */
#define SPP_ERROR_PACKET_LEN -2  /**< @brief Packet length too short */
#define SPP_ERROR_PAYLOAD_LEN_OUT_LIMITS                                       \
  -3                         /**< @brief Payload exceeds maximum size */
#define SPP_ERROR_VERSION -4 /**< @brief Invalid CCSDS version */
#define SPP_ERROR_MALFORMED_SEC_HEADER                                         \
  -5                                   /**< @brief Malformed secondary header */
#define SPP_ERROR_INVALID_BUFFER -6    /**< @brief Invalid input buffer */
#define SPP_ERROR_INVALID_APID -7      /**< @brief Invalid APID value */
#define SPP_ERROR_NULL_COUNTER -8      /**< @brief Packet counter NULL */
#define SPP_ERROR_NULL_SPACE_PACKET -9 /**< @brief Space Packet NULL */
#define SPP_ERROR_APID_OUT_OF_RANGE -10 /**< @brief APID Out of Range */
#endif                                  // LIBCCSDS_COMMON_H
