/* Copyright 2021 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <sifive_twinjet.h>
#include <stdio.h>

sifive_twinjet::sifive_twinjet(std::ostream &os, const fdt &dtb)
    : Device(os, dtb, "hwpf-nStreams"), is_present(false) {
  dtb.match(std::regex("cpu"), [&](const node n) {
    n.maybe_tuple(compat_string, tuple_t<uint32_t>(), []() {},
                  [&](uint32_t num) {
                    if (!is_present)
                      is_present = true;
                  });
  });
}

void sifive_twinjet::include_headers() {
  if (is_present)
    emit_include("sifive,twinjet");
}

void sifive_twinjet::declare_structs() {
  if (is_present)
    os << "extern struct __metal_driver_sifive_twinjet __metal_dt_twinjet;\n\n";
}

void sifive_twinjet::define_structs() {
  if (is_present) {
    os << "struct __metal_driver_sifive_twinjet __metal_dt_twinjet = {\n";
    emit_struct_field("twinjet.vtable",
                      "&__metal_driver_vtable_sifive_twinjet.twinjet");
    emit_struct_end();
  }
}

void sifive_twinjet::create_handles() {
  if (is_present)
    os << "#define __METAL_DT_TWINJET_HANDLE (&__metal_dt_twinjet)\n\n";
}
