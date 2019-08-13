/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_SPI0__H
#define __BARE_HEADER_SIFIVE_SPI0__H

#include "bare_header/device.h"

#include <regex>

class sifive_spi0 : public Device {
public:
  sifive_spi0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,spi0") {}

  void emit_defines() {
    dtb.match(std::regex(compat_string), [&](node n) {
      emit_comment(n);

      emit_base(n);
      emit_size(n);

      os << std::endl;
    });
  }

  void emit_offsets() {
    if (dtb.match(std::regex(compat_string), [](const node n) {}) != 0) {
      emit_compat();
      emit_offset("SCKDIV", 0x0);
      emit_offset("SCKMODE", 0x4);
      emit_offset("CSID", 0x10);
      emit_offset("CSDEF", 0x14);
      emit_offset("CSMODE", 0x18);
      emit_offset("DELAY0", 0x28);
      emit_offset("DELAY1", 0x2C);
      emit_offset("FMT", 0x40);
      emit_offset("TXDATA", 0x48);
      emit_offset("RXDATA", 0x4C);
      emit_offset("TXMARK", 0x50);
      emit_offset("RXMARK", 0x54);

      /* TODO: Only include if the device supports a direct-map flash interface
       */
      emit_offset("FCTRL", 0x60);
      emit_offset("FFMT", 0x64);

      emit_offset("IE", 0x70);
      emit_offset("IP", 0x74);

      os << std::endl;
    }
  }
};

#endif
