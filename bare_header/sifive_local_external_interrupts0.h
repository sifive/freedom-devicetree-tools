/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_LOCAL_EXTERNAL_INTERRUPTS0__H
#define __BARE_HEADER_SIFIVE_LOCAL_EXTERNAL_INTERRUPTS0__H

#include "bare_header/device.h"

#include <regex>

class sifive_local_external_interrupts0 : public Device {
public:
  sifive_local_external_interrupts0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,local-external-interrupts0") {}

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
