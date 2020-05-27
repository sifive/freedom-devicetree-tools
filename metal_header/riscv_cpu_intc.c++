/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <riscv_cpu_intc.h>

#include <regex>

using std::string;

riscv_cpu_intc::riscv_cpu_intc(std::ostream &os, const fdt &dtb)
    : Device(os, dtb, "riscv,cpu-intc") {}

string riscv_cpu_intc::handle(node n) {
  return n.parent().handle() + "_" + n.handle();
}

void riscv_cpu_intc::declare_structs() {
  dtb.match(std::regex(compat_string), [&](node n) {
    os << "extern struct __metal_driver_riscv_cpu_intc __metal_dt_" << handle(n)
       << ";\n\n";
  });
}

void riscv_cpu_intc::define_structs() {
  dtb.match(std::regex(compat_string), [&](node n) {
    emit_comment(n);
    os << "struct __metal_driver_riscv_cpu_intc __metal_dt_" << handle(n)
       << " = {\n";

    emit_struct_field(
        "controller.vtable",
        "&__metal_driver_vtable_riscv_cpu_intc.controller_vtable");
    emit_struct_field("init_done", "0");

    emit_struct_end();
  });
}
