# SPP Library for CCSDS Space Packet Protocol
## Overview
The **SPP Library** is a lightweight, portable C library implementing the CCSDS Space Packet Protocol (SPP) as per CCSDS 133.0-B-2. Developed for the Pwnsat satellite project, it provides functions to build and parse telemetry (TM) and telecommand (TC) packets, manage sequence counters, and handle idle packets for satellite communication. The library is designed to be generic, supporting microcontrollers (e.g., ESP32, RP2040) and desktop systems (Linux, Windows, macOS).
## Features
- **CCSDS Compliance**: Implements the CCSDS Space Packet Protocol (133.0-B-2) with a 6-byte primary header and variable-length packet data field.
- **Telemetry and Telecommand**: Supports building and parsing TM (SPP_PTYPE_TM) and TC (SPP_PTYPE_TC) packets.
- **Idle Packets**: Generates idle packets with APID 0x7FF and a fixed payload ("HackTheWorld" as part of the pwnsat implementation).
- **Secondary Header Support**: Allows optional secondary headers, with user-defined format and length.
Sequence Counting: Maintains separate 14-bit sequence counters for TM and TC packets, with automatic overflow reset at 16383.
- **Cross-Platform**: Pure C implementation using standard libraries (<stdint.h>, <string.h>), compatible with embedded and desktop systems.
- **Configurable Payload Size**: Supports payloads up to 200 bytes by default, adjustable via spp_config.h.

## Installation 
### Prerequisites
- A C compiler (e.g., GCC, Clang for desktop; ESP-IDF for ESP32; Pico SDK for RP2040).
- Standard C libraries (<stdint.h>, <string.h>).

### Directory Structure
```
spp_library/
├── include/
│   ├── spp.h          // Header file with definitions and prototypes
│   └── spp_config.h   // Configuration file for platform-specific settings
├── src/
│   └── spp.c          // Implementation of SPP functions
└── README.md
```
### Build Instructions
1. **Clone the Repository**:
```bash
git clone <repository-url>
cd spp_library
```
2. **Desktop (Linux/Windows/macOS)**:
   - Use the provided CMakeLists.txt:
    ```cmake
    cmake_minimum_required(VERSION 3.5)
    project(spp C)
    add_library(spp src/spp.c)
    target_include_directories(spp PUBLIC include)
    ```
   - Build:
    ```bash
    mkdir build && cd build
    cmake ..
    make
    ```

## Usage
### Configuration
Edit `include/spp_config.h` to adjust platform-specific settings:
```c
// [...]
/**
 * @brief Maximum payload size for a single packet chunk.
 *
 * Limits the payload (including secondary header) to 200 bytes to ensure
 * compatibility with LoRa communication constraints.
 */
#define SPP_MAX_PAYLOAD_CHUNK 200
```

### Example: Building a Telemetry Packet
```c
#include "spp.h"

void send_telemetry(void) {
    space_packet_t packet;
    uint8_t data[] = {0x01, 0x02, 0x03}; // Example payload
    int result = spp_tm_build_packet(&packet, SPP_GROUP_FLAG_UNSEGMENTED,
                                     SPP_SECHEAD_FLAG_NOPRESENT, 0, 100,
                                     data, 3);
    if (result == SPP_ERROR_NONE) {
        // Transmit packet (e.g., via LoRa on ESP32 or TCP on desktop)
        // Example: sx1262_transmit(packet.data, packet.header.length + SPP_PRIMARY_HEADER_LEN);
    }
}
```
### Example: Parsing a Packet
```c

#include "spp.h"

void process_packet(uint8_t* buffer, uint16_t len) {
    space_packet_t packet;
    int result = spp_unpack_packet(&packet, buffer, len);
    if (result == SPP_ERROR_NONE) {
        uint16_t apid = spp_get_apid(&packet);
        uint16_t seq_count = spp_get_sequence_count(&packet);
        // Process packet.data based on apid
    }
}
```
## Key Functions
- `spp_tm_build_packet`: Builds a telemetry packet with the specified APID and payload.
- `spp_tc_build_packet`: Builds a telecommand packet with the specified APID and payload.
- `spp_idle_build_packet`: Creates an idle packet with APID `0x7FF` and fixed payload.
- `spp_unpack_packet`: Parses a raw buffer into a `space_packet_t` structure.
- `spp_get_version`, `spp_get_type`, `spp_get_apid`, etc.: Extract header fields.

## Packet Structure
The library implements the CCSDS SPP packet format:
- **Primary Header** (6 bytes):
  - Version (3 bits): `CCSDS_SPP_VERSION = 0`
  - Type (1 bit): TM (`0`) or TC (`1`)
  - Secondary Header Flag (1 bit): Present (`1`) or Not Present (`0`)
  - APID (11 bits): 0–2047 (0x7FF for idle)
  - Sequence Flags (2 bits): Continuation (`00`), Start (`01`), End (`10`), Unsegmented (`11`)
  - Sequence Count (14 bits): 0–16383
  - Data Length (16 bits): Payload + secondary header length - 1
- **Data Field**: Up to 200 bytes (configurable), including optional secondary header and user data.

## Platform Support
- **Microcontrollers**:
  - **ESP32**: Compatible with ESP-IDF, suitable for LoRa (SX1262) communication in Pwnsat.
  - **RP2040**: Compatible with Pico SDK, ideal for lightweight embedded systems.
- **Desktop**:
  - **Linux/Windows/macOS**: Works with GCC/Clang for mission control applications (e.g., OpenC3 integration).

## Notes
- **Secondary Header**: Users must prepend the secondary header to the payload data and specify its length in `spp_tm_build_packet` or `spp_tc_build_packet`. The library assumes the secondary header is included in the data parameter.
- **Payload Size**: Default maximum is 200 bytes, suitable for LoRa constraints. Adjust `SPP_MAX_PAYLOAD_CHUNK` in `spp_config.h` for larger payloads (up to 65536 bytes per CCSDS).

## License
GNU GENERAL PUBLIC LICENSE v3.0

Copyright (C) 2025 Pwnsat

This program is free software: you can redistribute it and/or modify
it under the terms of the GNU General Public License as published by
the Free Software Foundation, either version 3 of the License, or
(at your option) any later version.

This program is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU General Public License for more details.

You should have received a copy of the GNU General Public License
along with this program.  If not, see <https://www.gnu.org/licenses/>.

## Contact
For questions or contributions, contact the Pwnsat team at [pwnsat.org](pwnsat.org).

