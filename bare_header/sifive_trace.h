/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_TRACE__H
#define __BARE_HEADER_SIFIVE_TRACE__H

#include "bare_header/device.h"

#include <regex>

class sifive_trace : public Device {
public:
  sifive_trace(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,trace") {}

  void emit_defines() {
    dtb.match(std::regex(compat_string), [&](node n) {
      emit_comment(n);

      emit_base(n);
      emit_size(n);

      /* Add more properties here */

      os << std::endl;
    });
  }

  void emit_offsets() {
    if (dtb.match(std::regex(compat_string), [](const node n) {}) != 0) {
      emit_compat();

      /* Add offsets here */

      emit_offset("TECONTROL", 0x0);
      emit_offset("TEIMPL", 0x4);
      emit_offset("TESINKBASE", 0x10);
      emit_offset("TESINKBASEHIGH", 0x14);
      emit_offset("TESINKBASELIMIT", 0x18);
      emit_offset("TESINKSINKWP", 0x1c);
      emit_offset("TESINKSINKRP", 0x20);
      emit_offset("TESINKSINKDATA", 0x24);
      emit_offset("TSCONTROL", 0x40);
      emit_offset("TSLOWER", 0x44);
      emit_offset("TSUPPER", 0x48);
      emit_offset("XTICONTROL", 0x50);
      emit_offset("XTOCONTROL", 0x54);
      emit_offset("WPCONTROL", 0x58);
      emit_offset("ITCTRACEENABLE", 0x60);
      emit_offset("ITCTRIGENABLE", 0x64);
      emit_offset("ITCSTIMULUS", 0x80);
      emit_offset("ATBSINK", 0x0e00);
      emit_offset("PBISINK", 0x0f00);

      os << std::endl;
    }
  }
};

#endif
