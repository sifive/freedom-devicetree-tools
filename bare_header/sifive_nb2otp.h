/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_NB2OTP__H
#define __BARE_HEADER_SIFIVE_NB2OTP__H

#include "bare_header/device.h"

#include <regex>
#include <set>

class sifive_nb2otp : public Device {
public:
  sifive_nb2otp(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,nb2otp") {}

  void emit_defines() override {
    dtb.match(std::regex(compat_string), [&](node n) {
      emit_comment(n);

      emit_base(n);
      emit_size(n);

      os << std::endl;
    });
  }


  void emit_offsets() override {
    if (dtb.match(std::regex(compat_string), [](const node n) {}) != 0) {
      emit_compat("sifive,nb2otp");

      emit_offset("PCSS_SCR_OTP_CONFG_0", 0x24);
      emit_offset("PCSS_SCR_OTP_1", 0x28);
      emit_offset("PCSS_SCR_OTP_2", 0x2C);
      emit_offset("PCSS_SCR_OTP_3", 0x30);
      emit_offset("PCSS_SCR_OTP_4", 0x34);
	  
      os << std::endl;
    }
  }
};

#endif
