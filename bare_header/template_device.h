/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

/* This is a device template. Copy and extend it to add more devices. */

#ifndef __BARE_HEADER_TEMPLATE_DEVICE__H
#define __BARE_HEADER_TEMPLATE_DEVICE__H

#include "bare_header/device.h"

#include <regex>

class template_device : public Device {
public:
  template_device(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "template_device") {}

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

      os << std::endl;
    }
  }
};

#endif
