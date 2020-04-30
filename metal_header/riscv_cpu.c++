/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <riscv_cpu.h>

riscv_cpu::riscv_cpu(std::ostream &os, const fdt &dtb)
    : Device(os, dtb, "cpu") {
  /* Count the number of CPUs */
  num_cpus = 0;
  dtb.match(std::regex(compat_string), [&](node n) { num_cpus += 1; });
}

void riscv_cpu::include_headers() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_include("riscv,cpu"); });
}

void riscv_cpu::declare_inlines() {
  Inline *func;
  std::list<Inline *> extern_inlines;
  int count = 0;

  dtb.match(std::regex(compat_string), [&](node n) {
    if (count == 0) {
      func = create_inline_dec("hartid", "int", "struct metal_cpu *cpu");
      extern_inlines.push_back(func);

      func = create_inline_dec("timebase", "int", "struct metal_cpu *cpu");
      extern_inlines.push_back(func);

      func =
          create_inline_dec("interrupt_controller", "struct metal_interrupt *",
                            "struct metal_cpu *cpu");
      extern_inlines.push_back(func);

      func =
          create_inline_dec("num_pmp_regions", "int", "struct metal_cpu *cpu");
      extern_inlines.push_back(func);

      func = create_inline_dec("buserror", "struct metal_buserror *",
                               "struct metal_cpu *cpu");
      extern_inlines.push_back(func);
    }
    count++;
  });
  os << "\n";
  os << "/* --------------------- cpu ------------ */\n";
  while (!extern_inlines.empty()) {
    func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_dec(func, "cpu");
    delete func;
  }
  os << "\n";
}

void riscv_cpu::define_inlines() {
  Inline *func_hartid;
  Inline *func_tf;
  Inline *func_ic;
  Inline *func_pmpregions;
  Inline *func_buserror;
  std::list<Inline *> extern_inlines;

  int count = 0;
  dtb.match(std::regex(compat_string), [&](node n) {
    /* Get the number of PMP regions from the CPU node */
    int num_pmp_regions;
    n.maybe_tuple("riscv,pmpregions", tuple_t<uint32_t>(),
                  [&]() { num_pmp_regions = 0; },
                  [&](uint32_t num) { num_pmp_regions = num; });

    std::string buserror_instance = "";
    n.maybe_tuple(
        "sifive,buserror", tuple_t<node>(),
        [&]() { buserror_instance = "NULL"; },
        [&](node n) { buserror_instance = "&__metal_dt_" + n.handle(); });

    /* Get the timebase frequency from the CPU node or its parent */
    int tf;
    n.maybe_tuple(
        "timebase-frequency", tuple_t<uint32_t>(),
        [&]() { tf = n.parent().get_field<uint32_t>("timebase-frequency"); },
        [&](uint32_t timebase) { tf = timebase; });

    if (count == 0) {
      func_hartid = create_inline_def(
          "hartid", "int",
          "(uintptr_t)cpu == (uintptr_t)&__metal_dt_" + n.handle(),
          n.instance(), "struct metal_cpu *cpu");
      extern_inlines.push_back(func_hartid);

      func_tf = create_inline_def("timebase", "int",
                                  "(uintptr_t)cpu == (uintptr_t)&__metal_dt_" +
                                      n.handle(),
                                  std::to_string(tf), "struct metal_cpu *cpu");
      extern_inlines.push_back(func_tf);

      func_ic = create_inline_def(
          "interrupt_controller", "struct metal_interrupt *",
          "(uintptr_t)cpu == (uintptr_t)&__metal_dt_" + n.handle(),
          "&__metal_dt_" + n.handle() + "_interrupt_controller.controller",
          "struct metal_cpu *cpu");
      extern_inlines.push_back(func_ic);

      func_pmpregions = create_inline_def(
          "num_pmp_regions", "int",
          "(uintptr_t)cpu == (uintptr_t)&__metal_dt_" + n.handle(),
          std::to_string(num_pmp_regions), "struct metal_cpu *cpu");
      extern_inlines.push_back(func_pmpregions);

      func_buserror = create_inline_def(
          "buserror", "struct metal_buserror *",
          "(uintptr_t)cpu == (uintptr_t)&__metal_dt_" + n.handle(),
          buserror_instance, "struct metal_cpu *cpu");
      extern_inlines.push_back(func_buserror);
    } else {
      add_inline_body(func_hartid,
                      "(uintptr_t)cpu == (uintptr_t)&__metal_dt_" + n.handle(),
                      n.instance());
      add_inline_body(func_tf,
                      "(uintptr_t)cpu == (uintptr_t)&__metal_dt_" + n.handle(),
                      std::to_string(tf));
      add_inline_body(
          func_ic, "(uintptr_t)cpu == (uintptr_t)&__metal_dt_" + n.handle(),
          "&__metal_dt_" + n.handle() + "_interrupt_controller.controller");
      add_inline_body(func_pmpregions,
                      "(uintptr_t)cpu == (uintptr_t)&__metal_dt_" + n.handle(),
                      std::to_string(num_pmp_regions));
      add_inline_body(func_buserror,
                      "(uintptr_t)cpu == (uintptr_t)&__metal_dt_" + n.handle(),
                      buserror_instance);
    }
    if ((count + 1) == num_cpus) {
      add_inline_body(func_hartid, "else", "-1");
      add_inline_body(func_tf, "else", "0");
      add_inline_body(func_ic, "else", "NULL");
      add_inline_body(func_pmpregions, "else", "0");
      add_inline_body(func_buserror, "else", "NULL");
    }
    count++;
  });
  os << "\n";
  os << "/* --------------------- cpu ------------ */\n";

  Inline *func;
  while (!extern_inlines.empty()) {
    func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_def(func, "cpu");
    delete func;
  }
  os << "\n";
}

void riscv_cpu::declare_structs() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_struct_decl("cpu", n); });
}

void riscv_cpu::define_structs() {
  dtb.match(std::regex(compat_string), [&](node n) {
    emit_struct_begin("cpu", n);

    emit_struct_field("cpu.vtable", "&__metal_driver_vtable_cpu.cpu_vtable");
    emit_struct_field("hpm_count", "0");
    emit_struct_end();
  });
}

void riscv_cpu::create_handles() {
  emit_def("__METAL_DT_MAX_HARTS", std::to_string(num_cpus));

  dtb.match(std::regex(compat_string), [&](node n) {
    auto name = n.name();
    if (n.field_exists("i-cache-size")) {
      emit_def("__METAL_" + n.handle_cap() + "_ICACHE_HANDLE",
               std::to_string(1));
    }
    if (n.field_exists("d-cache-size")) {
      emit_def("__METAL_" + n.handle_cap() + "_DCACHE_HANDLE",
               std::to_string(1));
    }
  });

  emit_struct_pointer_begin("cpu", "__metal_cpu_table", "[]");
  for (int i = 0; i < num_cpus; i++) {
    emit_struct_pointer_element("cpu", i, "",
                                ((i + 1) == num_cpus) ? "};\n\n" : ",\n");
  }
}
