/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <ucb_htif0.h>

ucb_htif0::ucb_htif0(std::ostream &os, const fdt &dtb)
    : Device(os, dtb, "ucb,htif0") {}

void ucb_htif0::include_headers() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_include(compat_string); });
}

void ucb_htif0::declare_structs() {
  dtb.match(std::regex(compat_string), [&](node n) {
    emit_struct_decl("ucb_htif0", "shutdown", n);
    emit_struct_decl("ucb_htif0", "uart", n);
  });
}

void ucb_htif0::define_structs() {
  dtb.match(std::regex(compat_string), [&](node n) {
    emit_struct_begin("ucb_htif0", "shutdown", n);
    emit_struct_field("vtable", "&__metal_driver_vtable_ucb_htif0_shutdown");
    emit_struct_field("shutdown.vtable",
                      "&__metal_driver_vtable_ucb_htif0_shutdown.shutdown");
    emit_struct_end();

    emit_struct_begin("ucb_htif0", "uart", n);
    emit_struct_field("vtable", "&__metal_driver_vtable_ucb_htif0_uart");
    emit_struct_field("uart.vtable",
                      "&__metal_driver_vtable_ucb_htif0_uart.uart");
    emit_struct_end();
  });
}

void ucb_htif0::create_handles() {
  dtb.match(std::regex(compat_string), [&](node n) {
    emit_def_handle("__METAL_DT_SHUTDOWN_HANDLE", "shutdown", n, ".shutdown");
  });
}
