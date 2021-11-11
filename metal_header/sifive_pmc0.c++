/* Copyright 2020 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <sifive_pmc0.h>

#include <regex>

sifive_pmc0::sifive_pmc0(std::ostream &os, const fdt &dtb)
    : Device(os, dtb, "sifive,pmc0") {}

void sifive_pmc0::include_headers() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_include("sifive,pmc0"); });
}
