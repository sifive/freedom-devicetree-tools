/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <riscv_plic0.h>

#include <regex>

riscv_plic0::riscv_plic0(std::ostream &os, const fdt &dtb)
    : Device(os, dtb, "riscv,plic0") {
  num_parents = 0;

  dtb.match(std::regex("riscv,plic0"), [&](node n) {
    n.maybe_tuple_size("interrupts-extended", tuple_t<node, uint32_t>(),
                       [&]() {},
                       [&](int s, node c, uint32_t line) { num_parents += 1; });
  });
}

void riscv_plic0::create_machine_macros() {
  dtb.match(std::regex("riscv,plic0"), [&](node n) {
    /* Add 1 to number of interrupts for 0 base software index */
    emit_def("__METAL_PLIC_SUBINTERRUPTS",
             std::to_string(n.get_field<uint32_t>("riscv,ndev") + 1));

    emit_def("__METAL_PLIC_NUM_PARENTS", std::to_string(num_parents));
  });

  /* If no PLIC exists, output 0 as a default value */
  os << "#ifndef __METAL_PLIC_SUBINTERRUPTS\n";
  os << "#define __METAL_PLIC_SUBINTERRUPTS 0\n";
  os << "#endif\n";
  os << "#ifndef __METAL_PLIC_NUM_PARENTS\n";
  os << "#define __METAL_PLIC_NUM_PARENTS 0\n";
  os << "#endif\n";
}

void riscv_plic0::create_defines() {
  uint32_t sub_interrupts = 0;

  dtb.match(std::regex(compat_string), [&](node n) {
    uint32_t num_interrupts =
        n.get_fields_count<std::tuple<node, uint32_t>>("interrupts-extended");

    emit_def("__METAL_" + n.handle_cap() + "_INTERRUPTS",
             std::to_string(num_interrupts));

    /* Add 1 to number of interrupts for 0 base software index */
    sub_interrupts = n.get_field<uint32_t>("riscv,ndev") + 1;
    emit_def("__METAL_PLIC_SUBINTERRUPTS", std::to_string(sub_interrupts));

    if (num_interrupts > max_interrupts) {
      max_interrupts = num_interrupts;
    }
  });
  if (sub_interrupts == 0) {
    os << "#define __METAL_PLIC_SUBINTERRUPTS 0\n";
  }
  emit_def("METAL_MAX_PLIC_INTERRUPTS", std::to_string(max_interrupts));

  emit_def("__METAL_PLIC_NUM_PARENTS", std::to_string(num_parents));
}

void riscv_plic0::include_headers() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_include(compat_string); });
}

void riscv_plic0::declare_inlines() {
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

      func = create_inline_dec("max_priority", "int",
                               "struct metal_interrupt *controller");
      extern_inlines.push_back(func);

      func = create_inline_dec("interrupt_parents", "struct metal_interrupt *",
                               "struct metal_interrupt *controller", "int idx");
      extern_inlines.push_back(func);

      func = create_inline_dec("interrupt_lines", "int",
                               "struct metal_interrupt *controller", "int idx");
      extern_inlines.push_back(func);

      func = create_inline_dec("context_ids", "int", "int hartid");
      extern_inlines.push_back(func);
    }
    count++;
  });
  os << "\n";
  os << "/* --------------------- sifive_plic0 ------------ */\n";
  while (!extern_inlines.empty()) {
    func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_dec(func, "sifive_plic0");
    delete func;
  }
  os << "\n";
}

#define MMODE_EXT_INTR 11

void riscv_plic0::define_inlines() {
  Inline *func;
  Inline *funcl;
  Inline *funcc;
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

      func = create_inline_def("max_priority", "int", "empty", "0",
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
            platform_define(n, METAL_RISCV_NDEV_LABEL),
            "struct metal_interrupt *controller");
        add_inline_body(func, "else", "0");
        extern_inlines.push_back(func);

        func = create_inline_def(
            "max_priority", "int",
            "(uintptr_t)controller == (uintptr_t)&__metal_dt_" + n.handle(),
            platform_define(n, METAL_RISCV_MAX_PRIORITY_LABEL),
            "struct metal_interrupt *controller");
        add_inline_body(func, "else", "0");
        extern_inlines.push_back(func);
      }
    }

    bool has_m_ext_intr = false;
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

            func = create_inline_def("context_ids", "int", "empty", "-1",
                                     "int hartid");
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

          if (irline == MMODE_EXT_INTR) {
            target_addr hartid = m.parent().get_field<target_addr>("reg");
            if (!has_m_ext_intr) {
              funcc = create_inline_def("context_ids", "int",
                                        "hartid == " + hartid.as_string(),
                                        std::to_string(i), "int hartid");
              extern_inlines.push_back(funcc);
              has_m_ext_intr = true;
            } else {
              add_inline_body(funcc, "hartid == " + hartid.as_string(),
                              std::to_string(i));
            }
          }

          if ((i + 1) == max_interrupts) {
            add_inline_body(func, "else", "NULL");
            add_inline_body(funcl, "else", "0");
            if (has_m_ext_intr)
              add_inline_body(funcc, "else", "-1");
          }
        });
    count++;
  });
  os << "\n";
  os << "/* --------------------- sifive_plic0 ------------ */\n";
  while (!extern_inlines.empty()) {
    func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_def(func, "sifive_plic0");
    delete func;
  }
  os << "\n";
}

void riscv_plic0::declare_structs() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_struct_decl("riscv_plic0", n); });
}

void riscv_plic0::define_structs() {
  dtb.match(std::regex(compat_string), [&](node n) {
    emit_struct_begin("riscv_plic0", n);

    emit_struct_field("controller.vtable",
                      "&__metal_driver_vtable_riscv_plic0.plic_vtable");
    emit_struct_field("init_done", "0");

    emit_struct_end();
  });
}

void riscv_plic0::create_handles() {
  dtb.match(std::regex(compat_string), [&](node n) {
    emit_def_handle("__METAL_DT_RISCV_PLIC0_HANDLE", n, ".controller");
  });
}
