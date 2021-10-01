/* Copyright 2021 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_PWM2__H
#define __BARE_HEADER_SIFIVE_PWM2__H

#include "bare_header/device.h"

#include <regex>

class sifive_pwm2 : public Device {
public:
  sifive_pwm2(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,pwm2") {}

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

      emit_offset("PWMCFG1", 0x0);
      emit_offset("PWMCFG2", 0x4);
      emit_offset("PWMCOUNTLO", 0x8);
      emit_offset("PWMCOUNTHI", 0xC);
      emit_offset("PWMS", 0x10);
      emit_offset("PWMCMP0", 0x20);
      emit_offset("PWMCMP1", 0x24);
      emit_offset("PWMCMP2", 0x28);
      emit_offset("PWMCMP3", 0x2C);
      emit_offset("PWMCMP4", 0x30);
      emit_offset("PWMCMP5", 0x34);
      emit_offset("PWMCMP6", 0x38);
      emit_offset("PWMCMP7", 0x3C);
      emit_offset("PWMRELO", 0x40);
      emit_offset("PWMRELH", 0x44);
      emit_offset("PWMMODESEL", 0x48);
      emit_offset("PWMINTR", 0x4C);
      emit_offset("PWMCAP0", 0x50);
      emit_offset("PWMCAP1", 0x54);
      emit_offset("PWMCAP2", 0x58);
      emit_offset("PWMCAP3", 0x5C);
      emit_offset("PWMCAP4", 0x60);
      emit_offset("PWMCAP5", 0x64);
      emit_offset("PWMCAP6", 0x68);
      emit_offset("PWMCAP7", 0x6C);

      os << std::endl;
    }
  }
};

#endif
