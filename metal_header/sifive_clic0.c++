/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <sifive_clic0.h>

#include <regex>

sifive_clic0::sifive_clic0(std::ostream &os, const fdt &dtb)
    : Device(os, dtb, "sifive,clic0") {}

void sifive_clic0::create_machine_macros() {
  dtb.match(std::regex("sifive,clic0"), [&](node n) {
    emit_def("__METAL_CLIC_SUBINTERRUPTS",
             std::to_string(n.get_field<uint32_t>("sifive,numints")));
  });

  /* If no CLIC exists, output 0 as a default value */
  os << "#ifndef __METAL_CLIC_SUBINTERRUPTS\n";
  os << "#define __METAL_CLIC_SUBINTERRUPTS 0\n";
  os << "#endif\n";
}

void sifive_clic0::create_defines() {
  uint32_t sub_interrupts = 0;

  dtb.match(std::regex(compat_string), [&](node n) {
    uint32_t num_interrupts =
        n.get_fields_count<std::tuple<node, uint32_t>>("interrupts-extended");
    emit_def("__METAL_" + n.handle_cap() + "_INTERRUPTS",
             std::to_string(num_interrupts));

    sub_interrupts = n.get_field<uint32_t>("sifive,numints");
    emit_def("__METAL_CLIC_SUBINTERRUPTS", std::to_string(sub_interrupts));

    if (num_interrupts > max_interrupts) {
      max_interrupts = num_interrupts;
    }
  });

  if (sub_interrupts == 0) {
    os << "#define __METAL_CLIC_SUBINTERRUPTS 0\n";
  }
  emit_def("METAL_MAX_CLIC_INTERRUPTS", std::to_string(max_interrupts));
}

void sifive_clic0::include_headers() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_include(compat_string); });
}

void sifive_clic0::declare_inlines() {
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

      func = create_inline_dec("interrupt_parent", "struct metal_interrupt *",
                               "struct metal_interrupt *controller");
      extern_inlines.push_back(func);

      func = create_inline_dec("num_interrupts", "int",
                               "struct metal_interrupt *controller");
      extern_inlines.push_back(func);

      func = create_inline_dec("interrupt_lines", "int",
                               "struct metal_interrupt *controller", "int idx");
      extern_inlines.push_back(func);

      func = create_inline_dec("max_levels", "int",
                               "struct metal_interrupt *controller");
      extern_inlines.push_back(func);

      func = create_inline_dec("num_subinterrupts", "int",
                               "struct metal_interrupt *controller");
      extern_inlines.push_back(func);

      func = create_inline_dec("num_intbits", "int",
                               "struct metal_interrupt *controller");
      extern_inlines.push_back(func);
    }
    count++;
  });
  os << "\n";
  os << "/* --------------------- sifive_clic0 ------------ */\n";
  while (!extern_inlines.empty()) {
    func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_dec(func, "sifive_clic0");
    delete func;
  }
  os << "\n";
}

void sifive_clic0::define_inlines() {
  Inline *func;
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
            "METAL_MAX_CLIC_INTERRUPTS", "struct metal_interrupt *controller");
        add_inline_body(func, "else", "0");
        extern_inlines.push_back(func);
      }
    }

    n.maybe_tuple_size(
        "interrupts-extended", tuple_t<node, uint32_t>(),
        [&]() {
          if (count == 0) {
            func = create_inline_def(
                "interrupt_parent", "struct metal_interrupt *", "empty", "NULL",
                "struct metal_interrupt *controller");
            extern_inlines.push_back(func);

            func = create_inline_def("interrupt_lines", "int", "empty", "0",
                                     "struct metal_interrupt *controller",
                                     "int idx");
            extern_inlines.push_back(func);
          }
        },
        [&](int s, node m, uint32_t irline) {
          static int i = 0;
          if (i == 0) {
            std::string value = "(struct metal_interrupt *)&__metal_dt_" +
                                m.parent().handle() + "_" + m.handle() +
                                ".controller";
            func = create_inline_def(
                "interrupt_parent", "struct metal_interrupt *",
                "(uintptr_t)controller == (uintptr_t)&__metal_dt_" + n.handle(),
                value, "struct metal_interrupt *controller");
            add_inline_body(func, "else", "NULL");
            extern_inlines.push_back(func);

            func = create_inline_def(
                "interrupt_lines", "int", "idx == " + std::to_string(i),
                std::to_string(irline), "struct metal_interrupt *controller",
                "int idx");
            extern_inlines.push_back(func);
            i++;
          }
          if (i == s) {
            add_inline_body(func, "idx == " + std::to_string(i),
                            std::to_string(irline));
            add_inline_body(func, "else", "0");
            i = 0;
          } else {
            add_inline_body(func, "idx == " + std::to_string(i),
                            std::to_string(irline));
            i++;
          }
        });

    if (max_interrupts == 0) {
      func = create_inline_def("max_levels", "int", "empty", "0",
                               "struct metal_interrupt *controller");
      extern_inlines.push_back(func);

      func = create_inline_def("num_subinterrupts", "int", "empty", "0",
                               "struct metal_interrupt *controller");
      extern_inlines.push_back(func);

      func = create_inline_def("num_intbits", "int", "empty", "0",
                               "struct metal_interrupt *controller");
      extern_inlines.push_back(func);
    } else {
      if (count == 0) {
        func = create_inline_def(
            "max_levels", "int",
            "(uintptr_t)controller == (uintptr_t)&__metal_dt_" + n.handle(),
            platform_define(n, METAL_SIFIVE_NUMLEVELS_LABEL),
            "struct metal_interrupt *controller");
        add_inline_body(func, "else", "0");
        extern_inlines.push_back(func);

        func = create_inline_def(
            "num_subinterrupts", "int",
            "(uintptr_t)controller == (uintptr_t)&__metal_dt_" + n.handle(),
            platform_define(n, METAL_SIFIVE_NUMINTS_LABEL),
            "struct metal_interrupt *controller");
        add_inline_body(func, "else", "0");
        extern_inlines.push_back(func);

        func = create_inline_def(
            "num_intbits", "int",
            "(uintptr_t)controller == (uintptr_t)&__metal_dt_" + n.handle(),
            platform_define(n, METAL_SIFIVE_NUMINTBITS_LABEL),
            "struct metal_interrupt *controller");
        add_inline_body(func, "else", "0");
        extern_inlines.push_back(func);
      }
    }

    count++;
  });
  os << "\n";
  os << "/* --------------------- sifive_clic0 ------------ */\n";
  while (!extern_inlines.empty()) {
    func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_def(func, "sifive_clic0");
    delete func;
  }
  os << "\n";
}

void sifive_clic0::declare_structs() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_struct_decl("sifive_clic0", n); });
}

void sifive_clic0::define_structs() {
  dtb.match(std::regex(compat_string), [&](node n) {
    emit_struct_begin("sifive_clic0", n);

    emit_struct_field("controller.vtable",
                      "&__metal_driver_vtable_sifive_clic0.clic_vtable");
    emit_struct_field("init_done", "0");

    emit_struct_end();
  });
}

void sifive_clic0::create_handles() {
  dtb.match(std::regex(compat_string), [&](node n) {
    emit_def_handle("__METAL_DT_SIFIVE_CLIC0_HANDLE", n, ".controller");
  });
}
