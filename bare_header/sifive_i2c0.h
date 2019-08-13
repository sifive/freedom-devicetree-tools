/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_I2C0__H
#define __BARE_HEADER_SIFIVE_I2C0__H

#include "bare_header/device.h"

#include <regex>

class sifive_i2c0 : public Device {
public:
  sifive_i2c0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,i2c0") {}

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

      emit_offset("PRESCALE_LOW", 0x0);
      emit_offset("PRESCALE_HIGH", 0x4);
      emit_offset("CONTROL", 0x8);

      emit_offset("TRANSMIT", 0xC);
      emit_offset("RECEIVE", 0xC);

      emit_offset("COMMAND", 0x10);
      emit_offset("STATUS", 0x10);

      os << std::endl;
    }
  }
};

#endif
