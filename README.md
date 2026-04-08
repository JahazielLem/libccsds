# LibCCSDS
---
[![Build Status](https://github.com/JahazielLem/libccsds/actions/workflows/build.yml/badge.svg)](https://github.com/JahazielLem/libccsds/actions)
[![License: GPL-3.0-or-later](https://img.shields.io/badge/License-GPL--3.0--or--later-blue.svg)](https://opensource.org/licenses/GPL-3.0)

A lightweight, modular C library for implementing **CCSDS (Consultative Committee for Space Data Systems)** communication protocols. Designed for embedded systems like the RP2040 and satellite simulators.

## Features
- **Space Packet Protocol (SPP)**: Fully compliant with CCSDS 133.0-B-2.
- **Memory Efficient**: Uses packed structures and static allocation where possible.
- **Multi-Instance**: Supports multiple APID contexts with independent sequence counters.
- **Safety**: Includes validation for packet lengths, versions, and buffer overflows.

## Directory Structure
```text
libccsds/
├── include/
│   └── ccsds/
│       ├── common.h         # Shared error codes and macros
│       └── spp/             # Space Packet Protocol module
├── src/
│   └── spp/                 # SPP source code
├── tests/                   # C-based unit tests
└── CMakeLists.txt           # Build system configuration
```
## Getting Started

**Prerequisites**
- CMake (3.10+)
- GCC / Arm GNU Toolchain (for embedded)

**Building the library**
```bash
mkdir build
cd build
cmake ..
make
```

**Running Tests**

```bash
ctest --output-on-failure
```

**Usage Example**
```c
#include <ccsds/spp/spp.h>

spp_apid_context_t my_apid = { .apid = 0x123, .tc = 0, .tm = 0 };
space_packet_t packet;
uint8_t data[] = { 0xDE, 0xAD, 0xBE, 0xEF };

int result = spp_tm_build_packet(&packet, SPP_GROUP_FLAG_UNSEGMENTED,
SPP_SECHEAD_FLAG_NOPRESENT, 0, data, sizeof(data), &my_apid);
```

## Roadmap
- [x] Space Packet Protocol (SPP)
- [ ] Telemetry (TM) Space Data Link Protocol
- [ ] Telecommand (TC) Space Data Link Protocol
- [ ] Advanced Orbiting Systems (AOS)

# License
This project is licensed under the GNU General Public License.
