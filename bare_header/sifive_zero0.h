/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_ZERO0__H
#define __BARE_HEADER_SIFIVE_ZERO0__H

#include "bare_header/device.h"

#include <regex>

class sifive_zero0 : public Device {
public:
  sifive_zero0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "ucbbar,cacheable-zero0") {}

  int get_index(const node &n) { return Device::get_index(n, compat_string); }

  void emit_defines() {
    dtb.match(std::regex(compat_string), [&](node n) {
      emit_comment(n);

      emit_base("ucbbar,cacheable-zero0", n);
      emit_size("ucbbar,cacheable-zero0", n);

      os << std::endl;
    });
  }
};

#endif
