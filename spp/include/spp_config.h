#ifndef __SPP_CONFIG_H
#define __SPP_CONFIG_H

/**
 * @brief Maximum payload size for a single packet chunk.
 *
 * Limits the payload (including secondary header) to 200 bytes to ensure
 * compatibility with LoRa communication constraints.
 */
#define SPP_MAX_PAYLOAD_CHUNK 248 // 255 Max - 6 bytes from - 1 bytes margin

#endif