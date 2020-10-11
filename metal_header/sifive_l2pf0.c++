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
  uint32_t count = 0, index = 0;
  std::string l2pf_base = "{\\\n\t\t\t\t";

  dtb.match(std::regex("cpu"), [&](node n) {
    if (n.field_exists("sifive,l2pf")) {
      node t = n.get_fields<node>("sifive,l2pf")[0];
      l2pf_base +=
          platform_define_offset(t, std::to_string(index) + "_BASE_ADDRESS");
      l2pf_base += ",\\\n\t\t\t\t";
      index++;
    } else {
      l2pf_base += "0UL,\\\n\t\t\t\t";
    }
  });
  l2pf_base += "}";

  dtb.match(std::regex(compat_string), [&](node n) {
    uint32_t val = 0;

    /* Generate defines for L2 prefetch parameters  */
    if (count == 0) {
      /* Array of base addresses with HART IDs as the index */
      emit_def("METAL_SIFIVE_L2PF0_BASE_ADDR", l2pf_base);

      if (n.field_exists("sifive,l2pf-queue-entries")) {
        val = n.get_fields<uint32_t>("sifive,l2pf-queue-entries")[0];
      }
      emit_def("METAL_SIFIVE_L2PF0_QUEUE_ENTRIES", std::to_string(val));

      val = 0;
      if (n.field_exists("sifive,l2pf-window-bits")) {
        val = n.get_fields<uint32_t>("sifive,l2pf-window-bits")[0];
      }
      emit_def("METAL_SIFIVE_L2PF0_WINDOW_BITS", std::to_string(val));

      val = 0;
      if (n.field_exists("sifive,l2pf-distance-bits")) {
        val = n.get_fields<uint32_t>("sifive,l2pf-distance-bits")[0];
      }
      emit_def("METAL_SIFIVE_L2PF0_DISTANCE_BITS", std::to_string(val));

      val = 0;
      if (n.field_exists("sifive,l2pf-streams")) {
        val = n.get_fields<uint32_t>("sifive,l2pf-streams")[0];
      }
      emit_def("METAL_SIFIVE_L2PF0_STREAMS", std::to_string(val));
      count++;
    }
  });
}
