/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_RTC0__H
#define __BARE_HEADER_SIFIVE_RTC0__H

#include "bare_header/device.h"

#include <regex>

class sifive_rtc0 : public Device {
  public:
    sifive_rtc0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,rtc0")
    {}

    int get_index(const node &n) override {
      return Device::get_index(n, "sifive,(rtc|aon0)");
    }

    void emit_defines() override {
      dtb.match(
	std::regex("sifive,(rtc0|aon0)"),
	[&](node n) {
	  emit_comment(n);

	  emit_base(n);
	  emit_size(n);

	  os << std::endl;
	});
    }

    void emit_offsets() override {
      if(dtb.match(std::regex("sifive,(rtc0|aon0)"), [](const node n){}) != 0) {
	emit_compat();

	emit_offset("RTCCFG", 0x40);
	emit_offset("RTCCOUNTLO", 0x48);
	emit_offset("RTCCOUNTHI", 0x4C);
	emit_offset("RTCS", 0x50);
	emit_offset("RTCCMP0", 0x60);

	os << std::endl;
      }
    }
};

#endif
