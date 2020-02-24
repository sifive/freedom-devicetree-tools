//* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <sifive_gpio_led.h>

#include <regex>

sifive_gpio_led::sifive_gpio_led(std::ostream &os, const fdt &dtb)
    : Device(os, dtb, "sifive,gpio-leds") {
  /* Count the number of LEDs */
  num_leds = 0;
  dtb.match(std::regex(compat_string), [&](node n) { num_leds += 1; });
}

void sifive_gpio_led::create_defines() {
  dtb.match(std::regex(compat_string), [&](node n) {});
}

void sifive_gpio_led::include_headers() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_include(compat_string); });
}

void sifive_gpio_led::declare_inlines() {
  Inline *func;
  std::list<Inline *> extern_inlines;
  int count = 0;

  dtb.match(std::regex(compat_string), [&](node n) {
    if (count == 0) {
      func = create_inline_dec("gpio", "struct metal_gpio *",
                               "struct metal_led *led");
      extern_inlines.push_back(func);

      func = create_inline_dec("pin", "int", "struct metal_led *led");
      extern_inlines.push_back(func);

      func = create_inline_dec("label", "char *", "struct metal_led *led");
      extern_inlines.push_back(func);
    }
    count++;
  });
  os << "\n";
  os << "/* --------------------- sifive_gpio_led ------------ */\n";
  while (!extern_inlines.empty()) {
    func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_retun_type(true, func->return_type);
    emit_inline_name("sifive_gpio_led", func->name);
    emit_inline_arg(os, func->args);
    os << ");\n";
    delete func;
  }
  os << "\n";
}

void sifive_gpio_led::define_inlines() {
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
                                     "NULL", "struct metal_led *led");
            extern_inlines.push_back(func);

            func = create_inline_def("pin", "int", "empty", "0",
                                     "struct metal_led *led");
            extern_inlines.push_back(func);
          }
        },
        [&](node m, uint32_t line) {
          if (count == 0) {
            func = create_inline_def(
                "gpio", "struct metal_gpio *",
                "(uintptr_t)led == (uintptr_t)&__metal_dt_" + n.handle(),
                "(struct metal_gpio *)&__metal_dt_" + m.handle(),
                "struct metal_led *led");
            extern_inlines.push_back(func);

            func1 = create_inline_def(
                "pin", "int",
                "(uintptr_t)led == (uintptr_t)&__metal_dt_" + n.handle(),
                std::to_string(line), "struct metal_led *led");
            extern_inlines.push_back(func1);
          }
          if (count > 0) {
            add_inline_body(
                func, "(uintptr_t)led == (uintptr_t)&__metal_dt_" + n.handle(),
                "(struct metal_gpio *)&__metal_dt_" + m.handle());
            add_inline_body(
                func1, "(uintptr_t)led == (uintptr_t)&__metal_dt_" + n.handle(),
                std::to_string(line));
          }
          if ((count + 1) == num_leds) {
            add_inline_body(func, "else", "NULL");
            add_inline_body(func1, "else", "0");
          }
        });

    if (num_leds == 0) {
      func2 = create_inline_def("label", "char *", "empty", "\"\"",
                                "struct metal_led *led");
      extern_inlines.push_back(func2);
    } else {
      if (count == 0) {
        func2 = create_inline_def(
            "label", "char *",
            "(uintptr_t)led == (uintptr_t)&__metal_dt_" + n.handle(),
            "\"" + n.get_field<std::string>("label") + "\"",
            "struct metal_led *led");
        extern_inlines.push_back(func2);
      }
      if (count > 0) {
        add_inline_body(
            func2, "(uintptr_t)led == (uintptr_t)&__metal_dt_" + n.handle(),
            "\"" + n.get_field<std::string>("label") + "\"");
      }
      if ((count + 1) == num_leds) {
        add_inline_body(func2, "else", "\"\"");
      }
    }
    count++;
  });
  os << "\n";
  os << "/* --------------------- sifive_gpio_led ------------ */\n";
  while (!extern_inlines.empty()) {
    func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_retun_type(false, func->return_type);
    emit_inline_name("sifive_gpio_led", func->name);
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

void sifive_gpio_led::declare_structs() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_struct_decl("sifive_gpio_led", n); });
}

void sifive_gpio_led::define_structs() {
  dtb.match(std::regex(compat_string), [&](node n) {
    emit_struct_begin("sifive_gpio_led", n);

    emit_struct_field("led.vtable",
                      "&__metal_driver_vtable_sifive_led.led_vtable");

    emit_struct_end();
  });
}

void sifive_gpio_led::create_handles() {
  emit_def("__METAL_DT_MAX_LEDS", std::to_string(num_leds));

  emit_struct_pointer_begin("sifive_gpio_led", "__metal_led_table", "[]");
  if (num_leds) {
    int i = 0;
    dtb.match(std::regex(compat_string), [&](node n) {
      os << "\t\t\t\t\t&__metal_dt_" << n.handle();

      if ((i + 1) == num_leds) {
        os << "};\n\n";
      } else {
        os << ",\n";
      }

      i++;
    });
  } else {
    emit_struct_pointer_end("NULL");
  }
}
