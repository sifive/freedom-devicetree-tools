/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_GPIO_BUTTONS__H
#define __BARE_HEADER_SIFIVE_GPIO_BUTTONS__H

#include "bare_header/device.h"

#include <regex>

class sifive_gpio_buttons : public Device {
public:
  sifive_gpio_buttons(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,gpio-buttons") {}

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
