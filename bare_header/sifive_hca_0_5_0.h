/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_HCA_0_5_0__H
#define __BARE_HEADER_SIFIVE_HCA_0_5_0__H

#include "bare_header/device.h"

#include <regex>
#include <set>

class sifive_hca_0_5_0 : public Device {
public:
  sifive_hca_0_5_0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,hca-0.5.0") {}

  void emit_defines() override {
    dtb.match(std::regex(compat_string), [&](node n) {
      emit_comment(n);

      emit_base("sifive,hca", n);
      emit_size("sifive,hca", n);

      os << std::endl;
    });
  }

  void emit_offsets() override {
    if (dtb.match(std::regex(compat_string), [](const node n) {}) != 0) {
      emit_compat();

      /* Add offsets here */
      emit_offset("sifive,hca", "VERSION", 0x000500);
      emit_offset("sifive,hca", "CR", 0x0);
      emit_offset("sifive,hca", "AES_CR", 0x10);
      emit_offset("sifive,hca", "AES_ALEN", 0x20);
      emit_offset("sifive,hca", "AES_PDLEN", 0x28);
      emit_offset("sifive,hca", "AES_KEY", 0x30);
      emit_offset("sifive,hca", "AES_INITV", 0x50);
      emit_offset("sifive,hca", "FIFO_IN", 0x70);
      emit_offset("sifive,hca", "AES_OUT", 0x80);
      emit_offset("sifive,hca", "AES_AUTH", 0x90);
      emit_offset("sifive,hca", "HASH", 0xA0);
      emit_offset("sifive,hca", "TRNG_CR", 0xE0);
      emit_offset("sifive,hca", "TRNG_SR", 0xE4);
      emit_offset("sifive,hca", "TRNG_DATA", 0xE8);
      emit_offset("sifive,hca", "TRNG_TRIM", 0xEC);
      emit_offset("sifive,hca", "DMA_CR", 0x110);
      emit_offset("sifive,hca", "DMA_LEN", 0x114);
      emit_offset("sifive,hca", "DMA_SRC", 0x118);
      emit_offset("sifive,hca", "DMA_DEST", 0x11C);
      emit_offset("sifive,hca", "HCA_REV", 0x200);
      emit_offset("sifive,hca", "AES_REV", 0x204);
      emit_offset("sifive,hca", "SHA_REV", 0x208);
      emit_offset("sifive,hca", "TRNG_REV", 0x20C);

      os << std::endl;
    }
  }
};

#endif
