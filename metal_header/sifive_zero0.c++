/* Copyright 2020 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <sifive_zero0.h>

#include <regex>

sifive_zero0::sifive_zero0(std::ostream &os, const fdt &dtb)
    : Device(os, dtb, "ucbbar,cacheable-zero0") {}

void sifive_zero0::include_headers() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_include("ucbbar,cacheable-zero0"); });
}
