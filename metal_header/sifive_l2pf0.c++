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

void sifive_l2pf0::create_defines() {
  dtb.match(std::regex(compat_string), [&](node n) {
    uint32_t val = 0;
    /* Generate defines for L2 prefetch parameters  */

    if (n.field_exists("sifive,spf-queue-entries")) {
      val = n.get_fields<uint32_t>("sifive,spf-queue-entries")[0];
    }
    emit_def("METAL_SIFIVE_L2PF0_QUEUE_ENTRIES", std::to_string(val));

    val = 0;
    if (n.field_exists("sifive,spf-window-bits")) {
      val = n.get_fields<uint32_t>("sifive,spf-window-bits")[0];
    }
    emit_def("METAL_SIFIVE_L2PF0_WINDOW_BITS", std::to_string(val));

    val = 0;
    if (n.field_exists("sifive,spf-distance-bits")) {
      val = n.get_fields<uint32_t>("sifive,spf-distance-bits")[0];
    }
    emit_def("METAL_SIFIVE_L2PF0_DISTANCE_BITS", std::to_string(val));

    val = 0;
    if (n.field_exists("sifive,spf-streams")) {
      val = n.get_fields<uint32_t>("sifive,spf-streams")[0];
    }
    emit_def("METAL_SIFIVE_L2PF0_STREAMS", std::to_string(val));
  });
}
