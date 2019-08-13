/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <sifive_gpio_switch.h>

#include <regex>

sifive_gpio_switch::sifive_gpio_switch(std::ostream &os, const fdt &dtb)
    : Device(os, dtb, "sifive,gpio-switches") {
  /* Count the number of SWITCHs */
  num_switches = 0;
  dtb.match(std::regex(compat_string), [&](node n) { num_switches += 1; });
}

void sifive_gpio_switch::create_defines() {
  dtb.match(std::regex(compat_string), [&](node n) {});
}

void sifive_gpio_switch::include_headers() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_include(compat_string); });
}

void sifive_gpio_switch::declare_inlines() {
  Inline *func;
  std::list<Inline *> extern_inlines;
  int count = 0;

  dtb.match(std::regex(compat_string), [&](node n) {
    if (count == 0) {
      func = create_inline_dec("gpio", "struct metal_gpio *",
                               "struct metal_switch *flip");
      extern_inlines.push_back(func);

      func = create_inline_dec("pin", "int", "struct metal_switch *flip");
      extern_inlines.push_back(func);

      func =
          create_inline_dec("interrupt_controller", "struct metal_interrupt *",
                            "struct metal_switch *flip");
      extern_inlines.push_back(func);

      func = create_inline_dec("interrupt_line", "int",
                               "struct metal_switch *flip");
      extern_inlines.push_back(func);

      func = create_inline_dec("label", "char *", "struct metal_switch *flip");
      extern_inlines.push_back(func);
    }
    count++;
  });
  os << "\n";
  os << "/* --------------------- sifive_gpio_switch ------------ */\n";
  while (!extern_inlines.empty()) {
    func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_retun_type(true, func->return_type);
    emit_inline_name("sifive_gpio_switch", func->name);
    emit_inline_arg(os, func->args);
    os << ");\n";
    delete func;
  }
  os << "\n";
}

void sifive_gpio_switch::define_inlines() {
  Inline *func;
  Inline *func1, *func2;
  std::list<Inline *> extern_inlines;

  int count = 0;
  dtb.match(std::regex(compat_string), [&](node n) {
    n.maybe_tuple(
        "gpios", tuple_t<node, uint32_t>(),
        [&]() {
          if (count == 0) {
            func = create_inline_def("gpio", "struct metal_gpio *", "empty",
                                     "NULL", "struct metal_switch *flip");
            extern_inlines.push_back(func);

            func = create_inline_def("pin", "int", "empty", "0",
                                     "struct metal_switch *flip");
            extern_inlines.push_back(func);
          }
        },
        [&](node m, uint32_t line) {
          if (count == 0) {
            func = create_inline_def(
                "gpio", "struct metal_gpio *",
                "(uintptr_t)flip == (uintptr_t)&__metal_dt_" + n.handle(),
                "(struct metal_gpio *)&__metal_dt_" + m.handle(),
                "struct metal_switch *flip");
            extern_inlines.push_back(func);

            func1 = create_inline_def(
                "pin", "int",
                "(uintptr_t)flip == (uintptr_t)&__metal_dt_" + n.handle(),
                std::to_string(line), "struct metal_switch *flip");
            extern_inlines.push_back(func1);
          }
          if (count > 0) {
            add_inline_body(
                func, "(uintptr_t)flip == (uintptr_t)&__metal_dt_" + n.handle(),
                "(struct metal_gpio *)&__metal_dt_" + m.handle());
            add_inline_body(func1,
                            "(uintptr_t)flip == (uintptr_t)&__metal_dt_" +
                                n.handle(),
                            std::to_string(line));
          }
          if ((count + 1) == num_switches) {
            add_inline_body(func, "else", "NULL");
            add_inline_body(func1, "else", "0");
          }
        });

    n.maybe_tuple(
        "interrupts-extended", tuple_t<node, uint32_t>(),
        [&]() {
          if (count == 0) {
            func = create_inline_def("interrupt_controller",
                                     "struct metal_interrupt *", "empty",
                                     "NULL", "struct metal_switch *flip");
            extern_inlines.push_back(func);

            func = create_inline_def("interrupt_line", "int", "empty", "0",
                                     "struct metal_switch *flip");
            extern_inlines.push_back(func);
          }
        },
        [&](node m, uint32_t line) {
          if (count == 0) {
            func = create_inline_def(
                "interrupt_controller", "struct metal_interrupt *",
                "(uintptr_t)flip == (uintptr_t)&__metal_dt_" + n.handle(),
                "(struct metal_interrupt *)&__metal_dt_" + m.handle(),
                "struct metal_switch *flip");
            extern_inlines.push_back(func);

            func1 = create_inline_def(
                "interrupt_line", "int",
                "(uintptr_t)flip == (uintptr_t)&__metal_dt_" + n.handle(),
                std::to_string(line), "struct metal_switch *flip");
            extern_inlines.push_back(func1);
          }
          if (count > 0) {
            add_inline_body(
                func, "(uintptr_t)flip == (uintptr_t)&__metal_dt_" + n.handle(),
                "(struct metal_interrupt *)&__metal_dt_" + m.handle());
            add_inline_body(func1,
                            "(uintptr_t)flip == (uintptr_t)&__metal_dt_" +
                                n.handle(),
                            std::to_string(line));
          }
          if ((count + 1) == num_switches) {
            add_inline_body(func, "else", "NULL");
            add_inline_body(func1, "else", "0");
          }
        });

    if (num_switches == 0) {
      func2 = create_inline_def("label", "char *", "empty", "\"\"",
                                "struct metal_switch *flip");
      extern_inlines.push_back(func2);
    } else {
      if (count == 0) {
        func2 = create_inline_def(
            "label", "char *",
            "(uintptr_t)flip == (uintptr_t)&__metal_dt_" + n.handle(),
            "\"" + n.get_field<std::string>("label") + "\"",
            "struct metal_switch *flip");
        extern_inlines.push_back(func2);
      }
      if (count > 0) {
        add_inline_body(
            func2, "(uintptr_t)flip == (uintptr_t)&__metal_dt_" + n.handle(),
            "\"" + n.get_field<std::string>("label") + "\"");
      }
      if ((count + 1) == num_switches) {
        add_inline_body(func2, "else", "\"\"");
      }
    }
    count++;
  });
  os << "\n";
  os << "/* --------------------- sifive_gpio_switch ------------ */\n";
  while (!extern_inlines.empty()) {
    func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_retun_type(false, func->return_type);
    emit_inline_name("sifive_gpio_switch", func->name);
    emit_inline_arg(os, func->args);
    os << ")\n";
    bool start = true;
    Inline::Stage stage;
    std::string bc, br;
    while (!func->body_cases.empty()) {
      bc = func->body_cases.front();
      func->body_cases.pop_front();
      br = func->body_returns.front();
      func->body_returns.pop_front();
      if (bc == "empty") {
        stage = Inline::Empty;
      } else if (bc == "else") {
        stage = Inline::End;
      } else {
        if (start == true) {
          stage = Inline::Start;
          start = false;
        } else {
          stage = Inline::Middle;
        }
      }
      emit_inline_body(stage, bc, br);
    }
    delete func;
  }
  os << "\n";
}

void sifive_gpio_switch::declare_structs() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_struct_decl("sifive_gpio_switch", n); });
}

void sifive_gpio_switch::define_structs() {
  dtb.match(std::regex(compat_string), [&](node n) {
    emit_struct_begin("sifive_gpio_switch", n);

    emit_struct_field("flip.vtable",
                      "&__metal_driver_vtable_sifive_switch.switch_vtable");

    emit_struct_end();
  });
}

void sifive_gpio_switch::create_handles() {
  emit_def("__METAL_DT_MAX_SWITCHES", std::to_string(num_switches));

  emit_struct_pointer_begin("sifive_gpio_switch", "__metal_switch_table", "[]");
  if (num_switches) {
    for (int i = 0; i < num_switches; i++) {
      emit_struct_pointer_element("switch", i, "",
                                  ((i + 1) == num_switches) ? "};\n\n" : ",\n");
    }
  } else {
    emit_struct_pointer_end("NULL");
  }
}
