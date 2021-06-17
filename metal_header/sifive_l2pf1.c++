/* Copyright 2021 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <sifive_l2pf1.h>

#include <regex>

sifive_l2pf1::sifive_l2pf1(std::ostream &os, const fdt &dtb)
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

void sifive_l2pf1::include_headers() {
  if (!skip) {
    dtb.match(std::regex(compat_string),
              [&](node n) { emit_include(compat_string); });
  }
}

void sifive_l2pf1::create_defines() {
  if (skip) {
    return;
  }

  uint32_t count = 0, index = 0;
  std::string l2pf_base = "{\\\n\t";
  std::string l2pf_queue_entries = "{\\\n\t";
  std::string l2pf_window_bits = "{\\\n\t";
  std::string l2pf_distance_bits = "{\\\n\t";
  std::string l2pf_streams = "{\\\n\t";

  dtb.match(std::regex("cpu"), [&](node n) {
    if (n.field_exists("sifive,l2pf")) {
      node t = n.get_fields<node>("sifive,l2pf")[0];
      l2pf_base += platform_define_offset(t, std::to_string(index) + "_" +
                                                 METAL_BASE_ADDRESS_LABEL);
      l2pf_base += ",\\\n\t";

      l2pf_queue_entries +=
          platform_define_offset(t, std::to_string(index) + "_QUEUE_ENTRIES");
      l2pf_queue_entries += ",\\\n\t";

      l2pf_window_bits +=
          platform_define_offset(t, std::to_string(index) + "_WINDOW_BITS");
      l2pf_window_bits += ",\\\n\t";

      l2pf_distance_bits +=
          platform_define_offset(t, std::to_string(index) + "_DISTANCE_BITS");
      l2pf_distance_bits += ",\\\n\t";

      l2pf_streams +=
          platform_define_offset(t, std::to_string(index) + "_STREAMS");
      l2pf_streams += ",\\\n\t";

      index++;
    } else {
      l2pf_base += "0UL,\\\n\t";
      l2pf_queue_entries += "0UL,\\\n\t";
      l2pf_window_bits += "0UL,\\\n\t";
      l2pf_distance_bits += "0UL,\\\n\t";
      l2pf_streams += "0UL,\\\n\t";
    }
  });
  l2pf_base += "}";
  l2pf_queue_entries += "}";
  l2pf_window_bits += "}";
  l2pf_distance_bits += "}";
  l2pf_streams += "}";

  dtb.match(std::regex(compat_string), [&](node n) {
    uint32_t val = 0;

    /* Generate defines for L2 stride prefetch parameters  */
    if (count == 0) {
      /* Array of configs with HART IDs as the index */
      emit_def("METAL_SIFIVE_L2PF1_BASE_ADDR", l2pf_base);
      emit_def("METAL_SIFIVE_L2PF1_QUEUE_ENTRIES", l2pf_queue_entries);
      emit_def("METAL_SIFIVE_L2PF1_WINDOW_BITS", l2pf_window_bits);
      emit_def("METAL_SIFIVE_L2PF1_DISTANCE_BITS", l2pf_distance_bits);
      emit_def("METAL_SIFIVE_L2PF1_STREAMS", l2pf_streams);
    }

    if (n.field_exists("sifive,l2pf-queue-entries")) {
      val = n.get_fields<uint32_t>("sifive,l2pf-queue-entries")[0];
    }
    emit_def("METAL_SIFIVE_L2PF1_" + std::to_string(count) + "_QUEUE_ENTRIES",
             std::to_string(val));

    val = 0;
    if (n.field_exists("sifive,l2pf-window-bits")) {
      val = n.get_fields<uint32_t>("sifive,l2pf-window-bits")[0];
    }
    emit_def("METAL_SIFIVE_L2PF1_" + std::to_string(count) + "_WINDOW_BITS",
             std::to_string(val));

    val = 0;
    if (n.field_exists("sifive,l2pf-distance-bits")) {
      val = n.get_fields<uint32_t>("sifive,l2pf-distance-bits")[0];
    }
    emit_def("METAL_SIFIVE_L2PF1_" + std::to_string(count) + "_DISTANCE_BITS",
             std::to_string(val));

    val = 0;
    if (n.field_exists("sifive,l2pf-streams")) {
      val = n.get_fields<uint32_t>("sifive,l2pf-streams")[0];
    }
    emit_def("METAL_SIFIVE_L2PF1_" + std::to_string(count) + "_STREAMS",
             std::to_string(val));

    count++;
  });
}
