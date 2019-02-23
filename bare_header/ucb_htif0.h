/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_UCB_HTIF0__H
#define __BARE_HEADER_UCB_HTIF0__H

#include "bare_header/device.h"

#include <regex>

class ucb_htif0 : public Device {
public:
  ucb_htif0(std::ostream &os, const fdt &dtb) : Device(os, dtb, "ucb,htif0") {}

  void emit_defines() {
    dtb.match(std::regex(compat_string), [&](node n) {
      emit_comment(n);

      os << std::endl;
    });
  }

  void emit_offsets() {
    if (dtb.match(std::regex(compat_string), [](const node n) {}) != 0) {
      emit_compat();

      os << std::endl;
    }
  }
};

#endif
