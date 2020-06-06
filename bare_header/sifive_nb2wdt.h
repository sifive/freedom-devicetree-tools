/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_NB2WDT__H
#define __BARE_HEADER_SIFIVE_NB2WDT__H

#include "bare_header/device.h"

#include <regex>
#include <set>

class sifive_nb2wdt : public Device {
public:
  sifive_nb2wdt(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,nb2wdt") {}

  void emit_defines() override {
    dtb.match(std::regex(compat_string), [&](node n) {
      emit_comment(n);

      emit_base(n);
      emit_size(n);

      os << std::endl;
    });
  }

#define WDT_BASE_ADDR           (0x2050E000UL)
#define WDT_CR_ADDR             (0x00000000UL)
#define WDT_TORR_ADDR           (0x00000004UL)
#define WDT_CCVR_ADDR           (0x00000008UL)
#define WDT_CRR_ADDR            (0x0000000CUL)
#define WDT_STAT_ADDR           (0x00000010UL)
#define WDT_EOI_ADDR            (0x00000014UL)
#define WDT_PROT_LEVEL_ADDR     (0x0000001CUL)
#define WDT_COMP_PARAM_1_ADDR   (0x000000F4UL)
#define WDT_COMP_PARAM_2_ADDR   (0x000000F0UL)
#define WDT_COMP_PARAM_3_ADDR   (0x000000ECUL)
#define WDT_COMP_PARAM_4_ADDR   (0x000000E8UL)
#define WDT_COMP_PARAM_5_ADDR   (0x000000E4UL)
#define WDT_COMP_VERSION_ADDR   (0x000000F8UL)
#define WDT_COMP_TYPE_ADDR      (0x000000FCUL)


  void emit_offsets() override {
    if (dtb.match(std::regex(compat_string), [](const node n) {}) != 0) {
      emit_compat("sifive,nb2wdt");

      emit_offset("WDT_ENABLE", 0x1);
      emit_offset("WDT_DISABLE", 0x0);
      emit_offset("WDT_RESTART", 0x76);
	  
      emit_offset("WDT_CR", 0x0);
      emit_offset("WDT_TORR", 0x4);
      emit_offset("WDT_CCVR", 0x8);
      emit_offset("WDT_CRR", 0xc);
      emit_offset("WDT_STAT", 0x10);
      emit_offset("WDT_EOI", 0x14);
	  
      os << std::endl;
    }
  }
};

#endif
