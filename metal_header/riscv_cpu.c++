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
  std::list<Inline *> extern_inlines;

  int count = 0;
  dtb.match(std::regex(compat_string), [&](node n) {
    /* Get the number of PMP regions from the CPU node */
    int num_pmp_regions;
    n.maybe_tuple("riscv,pmpregions", tuple_t<uint32_t>(),
                  [&]() { num_pmp_regions = 0; },
                  [&](uint32_t num) { num_pmp_regions = num; });

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
    }
    if ((count + 1) == num_cpus) {
      add_inline_body(func_hartid, "else", "-1");
      add_inline_body(func_tf, "else", "0");
      add_inline_body(func_ic, "else", "NULL");
      add_inline_body(func_pmpregions, "else", "0");
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

    emit_struct_end();
  });
}

#define DECLARE_CSR_INLINE_READ(CSR)                                           \
    func = create_inline_dec(#CSR"_read", "uintptr_t", "void");                \
    csr_inlines.push_back(func);
#define DECLARE_CSR_INLINE_READ_WRITE(CSR)                                     \
    func = create_inline_dec(#CSR"_read", "uintptr_t", "void");                \
    csr_inlines.push_back(func);                                               \
    func = create_inline_dec(#CSR"_write", "void", "uintptr_t value");         \
    csr_inlines.push_back(func);

void riscv_cpu::declare_csr_inlines() {
  Inline *func;
  std::list<Inline *> csr_inlines;

  for (int i = 0; i < 0xFFF; i++) {
    switch (i) {
    case 0x000:
      DECLARE_CSR_INLINE_READ_WRITE(ustatus)
      break;
    case 0xC00:
      DECLARE_CSR_INLINE_READ(cycle)
      break;
    case 0xC01:
      DECLARE_CSR_INLINE_READ(time)
      break;
    case 0x340:
      DECLARE_CSR_INLINE_READ_WRITE(mscratch)
      break;
    case 0xF11:
      DECLARE_CSR_INLINE_READ(mvendorid)
      break;
    case 0xF12:
      DECLARE_CSR_INLINE_READ(marchid)
      break;
    }
  }
  os << "\n";
  os << "/* --------------------- csr ------------ */\n";
  while (!csr_inlines.empty()) {
    func = csr_inlines.front();
    csr_inlines.pop_front();
    emit_inline_dec(func, "csr");
    delete func;
  }
  os << "\n";
}

#define DEFINE_CSR_INLINE_READ(CSR)                                            \
    func = create_inline_def(#CSR"_read", "uintptr_t",                         \
                             "uintptr_t m; __asm__ volatile (\"csrr %0, "#CSR  \
                             "\" : \"=r\"(m));",                               \
                             "m", "void");                                     \
    csr_inlines.push_back(func);
#define DEFINE_CSR_INLINE_READ_WRITE(CSR) \
    func = create_inline_def(#CSR"_read", "uintptr_t",                         \
                             "uintptr_t m; __asm__ volatile (\"csrr %0, "#CSR  \
                             "\" : \"=r\"(m));",                               \
                             "m", "void");                                     \
    csr_inlines.push_back(func);                                               \
    func = create_inline_def(#CSR"_write", "void",                             \
                             "__asm__ volatile (\"csrw "#CSR                   \
                             ", %0\" : : \"r\"(value));",                      \
                             "", "uintptr_t value");                           \
    csr_inlines.push_back(func);

void riscv_cpu::define_csr_inlines() {
  Inline *func;
  std::list<Inline *> csr_inlines;

  for (int i = 0; i < 0xFFF; i++) {
    switch (i) {
    case 0x000:
      DEFINE_CSR_INLINE_READ_WRITE(ustatus)
      break;
    case 0xC00:
      DEFINE_CSR_INLINE_READ(cycle)
      break;
    case 0xC01:
      DEFINE_CSR_INLINE_READ(time)
      break;
    case 0x340:
      DEFINE_CSR_INLINE_READ_WRITE(mscratch)
      break;
    case 0xF11:
      DEFINE_CSR_INLINE_READ(mvendorid)
      break;
    case 0xF12:
      DEFINE_CSR_INLINE_READ(marchid)
      break;
    }
  }
  os << "\n";
  os << "/* --------------------- csr ------------ */\n";
  while (!csr_inlines.empty()) {
    func = csr_inlines.front();
    csr_inlines.pop_front();
    emit_csr_def(func, "csr");
    delete func;
  }
  os << "\n";
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
