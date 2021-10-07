/* Copyright 2021 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_MALLARD0__H
#define __BARE_HEADER_SIFIVE_MALLARD0__H

#include "bare_header/device.h"

#include <regex>

class sifive_mallard0 : public Device {
private:
  bool skip;

public:
  sifive_mallard0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,mallard0"), skip(false) {}

  void emit_defines() {
    dtb.match(std::regex(compat_string),
              [&](const node n) {
                if (!skip)
                  emit_compat();
                skip = true;
              },
              true);
  }
};

#endif /* __BARE_HEADER_SIFIVE_MALLARD0__H */
