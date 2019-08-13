/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_PWM0__H
#define __BARE_HEADER_SIFIVE_PWM0__H

#include "bare_header/device.h"

#include <regex>

class sifive_pwm0 : public Device {
public:
  sifive_pwm0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,pwm0") {}

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

      emit_offset("PWMCFG", 0x0);
      emit_offset("PWMCOUNT", 0x8);
      emit_offset("PWMS", 0x10);
      emit_offset("PWMCMP0", 0x20);
      emit_offset("PWMCMP1", 0x24);
      emit_offset("PWMCMP2", 0x28);
      emit_offset("PWMCMP3", 0x2C);

      os << std::endl;
    }
  }
};

#endif
