/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

/* This is a device template. Copy and extend it to add more devices. */

#ifndef __BARE_HEADER_SIFIVE_FE310_G000_PRCI__H
#define __BARE_HEADER_SIFIVE_FE310_G000_PRCI__H

#include "bare_header/device.h"

#include <regex>

class sifive_fe310_g000_prci : public Device {
public:
  sifive_fe310_g000_prci(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,fe310-g000,prci") {}

  uint64_t base_address(const node &n) {
    uint64_t b;

    n.named_tuples("reg-names", "reg", "mem",
                   tuple_t<target_addr, target_size>(),
                   [&](target_addr base, target_size size) { b = base; });

    return b;
  }

  uint64_t size(const node &n) {
    uint64_t s;

    n.named_tuples("reg-names", "reg", "mem",
                   tuple_t<target_addr, target_size>(),
                   [&](target_addr base, target_size size) { s = size; });

    return s;
  }

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
      emit_offset(METAL_HFROSCCFG_LABEL, 0x0);
      emit_offset(METAL_HFXOSCCFG_LABEL, 0x4);
      emit_offset(METAL_PLLCFG_LABEL, 0x8);
      emit_offset(METAL_PLLOUTDIV_LABEL, 0xC);

      os << std::endl;
    }
  }
};

#endif
