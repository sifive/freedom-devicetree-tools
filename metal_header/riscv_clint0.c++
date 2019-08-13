/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <riscv_clint0.h>

#include <regex>

riscv_clint0::riscv_clint0(std::ostream &os, const fdt &dtb)
    : Device(os, dtb, "riscv,clint0") {
  num_parents = 0;

  dtb.match(std::regex("riscv,clint0"), [&](node n) {
    n.maybe_tuple_size("interrupts-extended", tuple_t<node, uint32_t>(),
                       [&]() {},
                       [&](int s, node c, uint32_t line) { num_parents += 1; });
  });
}

void riscv_clint0::create_machine_macros() {
  dtb.match(std::regex("riscv,clint0"), [&](node n) {
    emit_def("__METAL_CLINT_NUM_PARENTS", std::to_string(num_parents));
  });

  os << "#ifndef __METAL_CLINT_NUM_PARENTS\n";
  os << "#define __METAL_CLINT_NUM_PARENTS 0\n";
  os << "#endif\n";
}

void riscv_clint0::create_defines() {
  dtb.match(std::regex(compat_string), [&](node n) {
    uint32_t num_interrupts =
        n.get_fields_count<std::tuple<node, uint32_t>>("interrupts-extended");

    emit_def("__METAL_" + n.handle_cap() + "_INTERRUPTS",
             std::to_string(num_interrupts));

    if (num_interrupts > max_interrupts) {
      max_interrupts = num_interrupts;
    }
  });

  emit_def("METAL_MAX_CLINT_INTERRUPTS", std::to_string(max_interrupts));
  emit_def("__METAL_CLINT_NUM_PARENTS", std::to_string(num_parents));
}

void riscv_clint0::include_headers() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_include(compat_string); });
}

void riscv_clint0::declare_inlines() {
  Inline *func;
  std::list<Inline *> extern_inlines;
  int count = 0;

  dtb.match(std::regex(compat_string), [&](node n) {
    if (count == 0) {
      func = create_inline_dec("control_base", "unsigned long",
                               "struct metal_interrupt *controller");
      extern_inlines.push_back(func);

      func = create_inline_dec("control_size", "unsigned long",
                               "struct metal_interrupt *controller");
      extern_inlines.push_back(func);

      func = create_inline_dec("num_interrupts", "int",
                               "struct metal_interrupt *controller");
      extern_inlines.push_back(func);

      func = create_inline_dec("interrupt_parents", "struct metal_interrupt *",
                               "struct metal_interrupt *controller", "int idx");
      extern_inlines.push_back(func);

      func = create_inline_dec("interrupt_lines", "int",
                               "struct metal_interrupt *controller", "int idx");
      extern_inlines.push_back(func);
    }
    count++;
  });
  os << "\n";
  os << "/* --------------------- sifive_clint0 ------------ */\n";
  while (!extern_inlines.empty()) {
    func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_dec(func, "sifive_clint0");
    delete func;
  }
  os << "\n";
}

void riscv_clint0::define_inlines() {
  Inline *func;
  Inline *funcl;
  std::list<Inline *> extern_inlines;

  int count = 0;
  dtb.match(std::regex(compat_string), [&](node n) {
    if (max_interrupts == 0) {
      func = create_inline_def("control_base", "unsigned long", "empty", "0",
                               "struct metal_interrupt *controller");
      extern_inlines.push_back(func);

      func = create_inline_def("control_size", "unsigned long", "empty", "0",
                               "struct metal_interrupt *controller");
      extern_inlines.push_back(func);

      func = create_inline_def("num_interrupts", "int", "empty", "0",
                               "struct metal_interrupt *controller");
      extern_inlines.push_back(func);
    } else {
      if (count == 0) {
        func = create_inline_def(
            "control_base", "unsigned long",
            "(uintptr_t)controller == (uintptr_t)&__metal_dt_" + n.handle(),
            platform_define(n, METAL_BASE_ADDRESS_LABEL),
            "struct metal_interrupt *controller");
        add_inline_body(func, "else", "0");
        extern_inlines.push_back(func);

        func = create_inline_def(
            "control_size", "unsigned long",
            "(uintptr_t)controller == (uintptr_t)&__metal_dt_" + n.handle(),
            platform_define(n, METAL_SIZE_LABEL),
            "struct metal_interrupt *controller");
        add_inline_body(func, "else", "0");
        extern_inlines.push_back(func);

        func = create_inline_def(
            "num_interrupts", "int",
            "(uintptr_t)controller == (uintptr_t)&__metal_dt_" + n.handle(),
            "METAL_MAX_CLINT_INTERRUPTS", "struct metal_interrupt *controller");
        add_inline_body(func, "else", "0");
        extern_inlines.push_back(func);
      }
    }

    n.maybe_tuple_index(
        "interrupts-extended", tuple_t<node, uint32_t>(),
        [&]() {
          if (count == 0) {
            func = create_inline_def(
                "interrupt_parents", "struct metal_interrupt *", "empty",
                "NULL", "struct metal_interrupt *controller", "int idx");
            extern_inlines.push_back(func);

            func = create_inline_def("interrupt_lines", "int", "empty", "0",
                                     "struct metal_interrupt *controller",
                                     "int idx");
            extern_inlines.push_back(func);
          }
        },
        [&](int i, node m, uint32_t irline) {
          std::string value = "(struct metal_interrupt *)&__metal_dt_" +
                              m.parent().handle() + "_" + m.handle() +
                              ".controller";
          if (i == 0) {
            func = create_inline_def(
                "interrupt_parents", "struct metal_interrupt *",
                "idx == " + std::to_string(i), value,
                "struct metal_interrupt *controller", "int idx");
            extern_inlines.push_back(func);

            funcl = create_inline_def(
                "interrupt_lines", "int", "idx == " + std::to_string(i),
                std::to_string(irline), "struct metal_interrupt *controller",
                "int idx");
            extern_inlines.push_back(funcl);
          } else {
            add_inline_body(func, "idx == " + std::to_string(i), value);
            add_inline_body(funcl, "idx == " + std::to_string(i),
                            std::to_string(irline));
          }
          if ((i + 1) == max_interrupts) {
            add_inline_body(func, "else", "NULL");
            add_inline_body(funcl, "else", "0");
          }
        });

    count++;
  });
  os << "\n";
  os << "/* --------------------- sifive_clint0 ------------ */\n";
  while (!extern_inlines.empty()) {
    func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_def(func, "sifive_clint0");
    delete func;
  }
  os << "\n";
}

void riscv_clint0::declare_structs() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_struct_decl("riscv_clint0", n); });
}

void riscv_clint0::define_structs() {
  dtb.match(std::regex(compat_string), [&](node n) {
    emit_struct_begin("riscv_clint0", n);

    emit_struct_field("controller.vtable",
                      "&__metal_driver_vtable_riscv_clint0.clint_vtable");
    emit_struct_field("init_done", "0");

    emit_struct_end();
  });
}

void riscv_clint0::create_handles() {
  dtb.match(std::regex(compat_string), [&](node n) {
    emit_def_handle("__METAL_DT_RISCV_CLINT0_HANDLE", n, ".controller");
  });
}
