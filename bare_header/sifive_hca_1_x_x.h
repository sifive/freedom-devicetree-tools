/* Copyright 2021 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_HCA_1_x_x__H
#define __BARE_HEADER_SIFIVE_HCA_1_x_x__H

#include "bare_header/device.h"

#include <cstdio>
#include <regex>
#include <set>

class sifive_hca_1_x_x : public Device {
public:
  sifive_hca_1_x_x(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,hca-1.(\\d+).(\\d+)") {}

  void emit_defines() override {
    int option = 0;
    dtb.match(std::regex(compat_string), [&](node n) {
      string instance = n.get_fields<string>("compatible")[0];
      int major, minor, patch;

      emit_comment(n);
      int ret = std::sscanf(instance.c_str(), "sifive,hca-%d.%d.%d", &major,
                            &minor, &patch);
      if (ret == 3) {
        os << "#define METAL_SIFIVE_HCA" << major << std::endl;
        emit_offset("sifive,hca1", "VERSION",
                    (major << 16) + (minor << 8) + patch);
        os << std::endl;
      }

      emit_base("sifive,hca1", n);
      emit_size("sifive,hca1", n);

      os << std::endl;
    });
    dtb.match(std::regex("sifive,hca-aes"), [&](node n) {
      int base;
      int ret = std::sscanf(n.name().c_str(), "aes@%x", &base);
      if (ret == 1) {
        os << "#define METAL_SIFIVE_HCA1_AES_BASE_ADDRESS " << base << "UL"
           << std::endl;
        option = 1;
      }
      if (n.field_exists("aesmac_present")) {
        os << "#define METAL_SIFIVE_HCA1_AES_HAS_AESMAC " << std::endl;
      }
    });
    dtb.match(std::regex("sifive,hca-sha"), [&](node n) {
      int base;
      int ret = std::sscanf(n.name().c_str(), "sha@%x", &base);
      if (ret == 1) {
        os << "#define METAL_SIFIVE_HCA1_SHA_BASE_ADDRESS " << base << "UL"
           << std::endl;
        option = 1;
      }
    });
    dtb.match(std::regex("sifive,hca-pka"), [&](node n) {
      int base;
      int ret = std::sscanf(n.name().c_str(), "pka@%x", &base);
      if (ret == 1) {
        os << "#define METAL_SIFIVE_HCA1_PKA_BASE_ADDRESS " << base << "UL"
           << std::endl;
        option = 1;
      }
    });
    dtb.match(std::regex("sifive,hca-trng"), [&](node n) {
      int base;
      int ret = std::sscanf(n.name().c_str(), "trng@%x", &base);
      if (ret == 1) {
        os << "#define METAL_SIFIVE_HCA1_TRNG_BASE_ADDRESS " << base << "UL"
           << std::endl;
        option = 1;
      }
    });
    if (option == 1) {
      os << std::endl;
    }
  }
};

#endif
