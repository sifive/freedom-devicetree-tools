/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <riscv_pmp.h>

#include <regex>

riscv_pmp::riscv_pmp(std::ostream &os, const fdt &dtb)
    : Device(os, dtb, "riscv,pmpregions") {
  /* Check all RISC-V CPU nodes for the presence of the 'riscv,pmpregions'
   * property */
  pmp_present = 0;
  dtb.match(std::regex("cpu"), [&](const node n) {
    n.maybe_tuple(compat_string, tuple_t<uint32_t>(), []() {},
                  [&](uint32_t num) { pmp_present = 1; });
  });
}

void riscv_pmp::include_headers() { os << "#include <metal/pmp.h>\n"; }

void riscv_pmp::declare_structs() {
  if (pmp_present) {
    os << "extern struct metal_pmp __metal_dt_pmp;\n\n";
  } else if (dtb.match(std::regex("riscv,pmp"), [](const node n) {}) != 0) {
    /* This handles the old-style nodes with the riscv,pmp compat strings. */
    os << "extern struct metal_pmp __metal_dt_pmp;\n\n";
  }
}

void riscv_pmp::define_structs() {
  if (pmp_present) {
    os << "struct metal_pmp __metal_dt_pmp;\n\n";
  } else if (dtb.match(std::regex("riscv,pmp"), [](const node n) {}) != 0) {
    /* This handles the old-style nodes with the riscv,pmp compat strings. */
    os << "struct metal_pmp __metal_dt_pmp;\n\n";
  }
}

void riscv_pmp::create_handles() {
  if (pmp_present) {
    os << "#define __METAL_DT_PMP_HANDLE (&__metal_dt_pmp)\n\n";
  } else if (dtb.match(std::regex("riscv,pmp"), [](const node n) {}) != 0) {
    /* This handles the old-style nodes with the riscv,pmp compat strings. */
    os << "#define __METAL_DT_PMP_HANDLE (&__metal_dt_pmp)\n\n";
  }
}
