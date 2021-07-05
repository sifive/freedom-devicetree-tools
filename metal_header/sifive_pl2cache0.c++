/* Copyright 2020 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <sifive_pl2cache0.h>

#include <regex>

sifive_pl2cache0::sifive_pl2cache0(std::ostream &os, const fdt &dtb)
    : Device(os, dtb, "sifive,pL2Cache0") {}

void sifive_pl2cache0::include_headers() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_include("sifive,pl2cache0"); });
}

void sifive_pl2cache0::create_defines() {
  std::string pl2cache_base = "{\\\n\t\t\t\t";

  dtb.match(std::regex("cpu"), [&](node n) {
    bool found = false;
    if (n.field_exists("next-level-cache")) {
      node t = n.get_fields<node>("next-level-cache")[0];

      auto compat_strings = t.get_fields<std::string>("compatible");
      for (auto it = compat_strings.begin();
           !found && it != compat_strings.end(); it++) {
        if (it->compare("sifive,pL2Cache0") == 0) {
          uint32_t idx = 0;
          dtb.match(std::regex(compat_string), [&](node p) {
            if (p.handle() == t.handle()) {
              found = true;
              pl2cache_base += platform_define_offset(t, std::to_string(idx) +
                                                             "_BASE_ADDRESS");
              pl2cache_base += ",\\\n\t\t\t\t";
            }
            idx++;
          });
        }
      }
    }
    if (!found) {
      pl2cache_base += "0UL,\\\n\t\t\t\t";
    }
  });
  pl2cache_base += "}";

  uint32_t count = 0;
  dtb.match(std::regex(compat_string), [&](node n) {
    if (count == 0) {
      emit_def("METAL_PL2CACHE_DRIVER_PREFIX", "sifive_pl2cache0");
      /* Array of base addresses with HART IDs as the index */
      emit_def("METAL_SIFIVE_PL2CACHE0_BASE_ADDR", pl2cache_base);

      uint32_t perfmon_count = 0;

      if (n.field_exists("sifive,perfmon-counters")) {
        /* Get available L2 cache performance counters */
        perfmon_count = n.get_fields<uint32_t>("sifive,perfmon-counters")[0];
      }
      emit_def("METAL_SIFIVE_PL2CACHE0_PERFMON_COUNTERS",
               std::to_string(perfmon_count));

      /* Set handle as NULL, for API compatibility */
      emit_def("__METAL_DT_SIFIVE_PL2CACHE0_HANDLE",
               "(struct metal_cache *)NULL");
      count++;
    }
  });
}
