/* Copyright 2020 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <sifive_l2pf0.h>

#include <regex>

sifive_l2pf0::sifive_l2pf0(std::ostream &os, const fdt &dtb)
    : Device(os, dtb, "sifive,l2pf0") {}

void sifive_l2pf0::include_headers() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_include(compat_string); });
}
