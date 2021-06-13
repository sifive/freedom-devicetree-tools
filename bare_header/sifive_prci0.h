/* Copyright 2021 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_PRCI0__H
#define __BARE_HEADER_SIFIVE_PRCI0__H

#include "bare_header/device.h"

#include <regex>

#define PRCI_PREFIX "sifive,prci0"

class sifive_prci0 : public Device {
public:
  sifive_prci0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,(fu540|fu740)-c000-prci0") {
    dtb.match(std::regex(compat_string), [&](node n) {
      compat_string = n.get_fields<string>("compatible")[0];
    });
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
      if (compat_string.find("fu540") != std::string::npos) {
        emit_offset(PRCI_PREFIX, "HFXOSCCFG", 0x0);
        emit_offset(PRCI_PREFIX, "COREPLLCFG", 0x4);
        emit_offset(PRCI_PREFIX, "COREPLLOUTDIV", 0x8);
        emit_offset(PRCI_PREFIX, "DDRPLLCFG", 0xC);
        emit_offset(PRCI_PREFIX, "DDRPLLOUTDIV", 0x10);
        emit_offset(PRCI_PREFIX, "GEMGXLPLLCFG", 0x1C);
        emit_offset(PRCI_PREFIX, "GEMGXLPLLOUTDIV", 0x20);
        emit_offset(PRCI_PREFIX, "CORECLKSELREG", 0x24);
        emit_offset(PRCI_PREFIX, "DEVICESRESETN", 0x28);
        emit_offset(PRCI_PREFIX, "CLKMUXSTATUS", 0x2C);
        emit_offset(PRCI_PREFIX, "PROCMONCFG", 0xF0);
      } else if (compat_string.find("fu740") != std::string::npos) {
        emit_offset(PRCI_PREFIX, "HFXOSCCFG", 0x0);
        emit_offset(PRCI_PREFIX, "COREPLLCFG", 0x4);
        emit_offset(PRCI_PREFIX, "COREPLLOUTDIV", 0x8);
        emit_offset(PRCI_PREFIX, "DDRPLLCFG", 0xC);
        emit_offset(PRCI_PREFIX, "DDRPLLOUTDIV", 0x10);
        emit_offset(PRCI_PREFIX, "GEMGXLPLLCFG", 0x1C);
        emit_offset(PRCI_PREFIX, "GEMGXLPLLOUTDIV", 0x20);
        emit_offset(PRCI_PREFIX, "CORECLKSELREG", 0x24);
        emit_offset(PRCI_PREFIX, "DEVICESRESETN", 0x28);
        emit_offset(PRCI_PREFIX, "CLKMUXSTATUS", 0x2C);
        emit_offset(PRCI_PREFIX, "CLTXPLLCFG", 0x30);
        emit_offset(PRCI_PREFIX, "CLTXPLLOUTDIV", 0x34);
        emit_offset(PRCI_PREFIX, "DVFSCOREPLL", 0x38);
        emit_offset(PRCI_PREFIX, "DVFSCOREPLLOUTDIV", 0x3C);
        emit_offset(PRCI_PREFIX, "COREPLLSEL", 0x40);
        emit_offset(PRCI_PREFIX, "HFPCLKPLLCFG", 0x50);
        emit_offset(PRCI_PREFIX, "HFPCLKPLLOUTDIV", 0x54);
        emit_offset(PRCI_PREFIX, "HFPCLKPLLSEL", 0x58);
        emit_offset(PRCI_PREFIX, "HFPCLKDIVREG", 0x5C);
        emit_offset(PRCI_PREFIX, "PRCIPLLS", 0xE0);
        emit_offset(PRCI_PREFIX, "PROCMONCFG", 0xF0);
      }
      os << std::endl;
    }
  };
};

#endif /* __BARE_HEADER_SIFIVE_PRCI0__H */
