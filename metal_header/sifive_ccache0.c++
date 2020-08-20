/* Copyright 2020 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <sifive_ccache0.h>

#include <regex>

sifive_ccache0::sifive_ccache0(std::ostream &os, const fdt &dtb)
    : Device(os, dtb, "sifive,(ccache0|fu540-c000,l2)") {}

void sifive_ccache0::include_headers() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_include("sifive,ccache0"); });
}

void sifive_ccache0::create_defines() {

  dtb.match(std::regex(compat_string), [&](node n) {
    /* Get list of interrupt IDs */
    std::string interrupt_arr = "{ ";
    for (int i = 0; i < n.get_fields_count<uint32_t>("interrupts"); i++) {
      interrupt_arr += std::to_string(n.get_fields<uint32_t>("interrupts")[i]);
      interrupt_arr += ", ";
    }
    interrupt_arr += "}";

    emit_def("METAL_SIFIVE_CCACHE0_INTERRUPTS", interrupt_arr);

    /* Interrupt parent controller */
    std::string int_parent_value = "NULL";
    n.maybe_tuple("interrupt-parent", tuple_t<node>(), [&]() {},
                  [&](node m) {
                    int_parent_value =
                        "&__metal_dt_" + m.handle() + ".controller";
                  });

    emit_def("METAL_SIFIVE_CCACHE0_INTERRUPT_PARENT", int_parent_value);

    emit_def("METAL_CACHE_DRIVER_PREFIX", "sifive_ccache0");

    uint32_t perfmon_count = 0;

    if (n.field_exists("sifive,perfmon-counters")) {
      /* Get available L2 cache performance counters */
      perfmon_count = n.get_fields<uint32_t>("sifive,perfmon-counters")[0];
    }
    emit_def("METAL_SIFIVE_CCACHE0_PERFMON_COUNTERS",
             std::to_string(perfmon_count));

    /* Set handle as NULL, for API compatibility */
    emit_def("__METAL_DT_SIFIVE_CCACHE0_HANDLE", "(struct metal_cache *)NULL");
  });
}
