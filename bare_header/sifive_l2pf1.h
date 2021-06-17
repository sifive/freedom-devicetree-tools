/* Copyright 2021 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_L2PF1__H
#define __BARE_HEADER_SIFIVE_L2PF1__H

#include "bare_header/device.h"

#include <regex>

class sifive_l2pf1 : public Device {
private:
  bool skip;

public:
  sifive_l2pf1(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,l2pf1"), skip(false) {
    bool l2pf_compat = false;
    dtb.match(std::regex("cpu"), [&](node n) {
      if (!skip && !l2pf_compat && n.field_exists("sifive,l2pf")) {
        node t = n.get_fields<node>("sifive,l2pf")[0];
        auto compat_strings = t.get_fields<std::string>("compatible");
        for (auto it = compat_strings.begin();
             !skip && !l2pf_compat && it != compat_strings.end(); it++) {
          std::regex lp2f_regex("sifive,l2pf\\d+");
          std::smatch l2pf_match;
          std::regex_match(*it, l2pf_match, lp2f_regex);
          if (!l2pf_match.empty()) {
            l2pf_compat = true;
            if (l2pf_match[0].str() != compat_string) {
              skip = true;
              break;
            }
          }
        }
      }
    });
  }

  int get_index(const node &n) { return Device::get_index(n, compat_string); }

  void emit_defines() {
    if (!skip) {
      dtb.match(std::regex(compat_string), [&](node n) {
        emit_comment(n);

        emit_base(n);
        emit_size(n);

        os << std::endl;
      });
    }
  }

  void emit_offsets() {
    if (!skip) {
      if (dtb.match(std::regex(compat_string), [](const node n) {}) != 0) {
        emit_compat();
        emit_offset("BASIC_CONTROL", 0x00);
        emit_offset("USER_CONTROL", 0x04);

        os << std::endl;
      }
    }
  }
};

#endif
