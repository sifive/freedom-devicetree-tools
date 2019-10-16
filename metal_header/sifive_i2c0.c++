/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <sifive_i2c0.h>

sifive_i2c0::sifive_i2c0(std::ostream &os, const fdt &dtb)
    : Device(os, dtb, "sifive,i2c0") {
  /* Count the number of I2Cs */
  num_i2cs = 0;
  dtb.match(std::regex(compat_string), [&](node n) { num_i2cs += 1; });
}

void sifive_i2c0::create_defines() {
  int i = 0;
  dtb.match(std::regex(compat_string), [&](node n) {
    uint32_t num_interrupts = n.get_fields_count<uint32_t>("interrupts");
    emit_def("__METAL_" + n.handle_cap() + "_INTERRUPTS",
             std::to_string(num_interrupts));

    if (num_interrupts > max_interrupts) {
      max_interrupts = num_interrupts;
    }
    i++;
  });
  emit_def("METAL_MAX_I2C0_INTERRUPTS", std::to_string(max_interrupts));
}

void sifive_i2c0::include_headers() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_include(compat_string); });
}

void sifive_i2c0::declare_inlines() {
  Inline *func;
  std::list<Inline *> extern_inlines;
  int count = 0;

  dtb.match(std::regex(compat_string), [&](node n) {
    if (count == 0) {
      func = create_inline_dec("control_base", "unsigned long",
                               "struct metal_i2c *i2c");
      extern_inlines.push_back(func);

      func = create_inline_dec("control_size", "unsigned long",
                               "struct metal_i2c *i2c");
      extern_inlines.push_back(func);

      func =
          create_inline_dec("num_interrupts", "int", "struct metal_i2c *i2c");
      extern_inlines.push_back(func);

      func = create_inline_dec("interrupt_parent", "struct metal_interrupt *",
                               "struct metal_i2c *i2c");
      extern_inlines.push_back(func);

      func =
          create_inline_dec("interrupt_line", "int", "struct metal_i2c *i2c");
      extern_inlines.push_back(func);

      func = create_inline_dec("clock", "struct metal_clock *",
                               "struct metal_i2c *i2c");
      extern_inlines.push_back(func);

      func = create_inline_dec("pinmux", "struct __metal_driver_sifive_gpio0 *",
                               "struct metal_i2c *i2c");
      extern_inlines.push_back(func);

      func = create_inline_dec("pinmux_output_selector", "unsigned long",
                               "struct metal_i2c *i2c");
      extern_inlines.push_back(func);

      func = create_inline_dec("pinmux_source_selector", "unsigned long",
                               "struct metal_i2c *i2c");
      extern_inlines.push_back(func);
    }
    count++;
  });
  os << "\n";
  os << "/* --------------------- sifive_i2c0 ------------ */\n";
  while (!extern_inlines.empty()) {
    func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_dec(func, "sifive_i2c0");
    delete func;
  }
  os << "\n";
}

void sifive_i2c0::define_inlines() {

  Inline *control_base_func;
  Inline *control_size_func;
  Inline *num_interrupts_func;
  Inline *clock_func;
  Inline *pinmux_func;
  Inline *pinmux_output_selector_func;
  Inline *pinmux_source_selector_func;
  Inline *interrupt_parent_func;
  Inline *interrupt_line_func;

  int count = 0;

  dtb.match(std::regex(compat_string), [&](node n) {
    /* Clock driving the i2c peripheral */
    std::string clock_value = "NULL";
    n.maybe_tuple("clocks", tuple_t<node>(), [&]() {},
                  [&](node m) {
                    clock_value = "(struct metal_clock *)&__metal_dt_" +
                                  m.handle() + ".clock";
                  });

    /* Pinmux */
    std::string pinmux_value = "NULL";
    uint32_t pinmux_dest = 0;
    uint32_t pinmux_source = 0;
    n.maybe_tuple("pinmux", tuple_t<node, uint32_t, uint32_t>(), [&]() {},
                  [&](node m, uint32_t dest, uint32_t source) {
                    pinmux_value =
                        "(struct __metal_driver_sifive_gpio0 *)&__metal_dt_" +
                        m.handle();
                    pinmux_dest = dest;
                    pinmux_source = source;
                  });

    /* Interrupt parent controller */
    std::string int_parent_value = "NULL";
    n.maybe_tuple("interrupt-parent", tuple_t<node>(), [&]() {},
                  [&](node m) {
                    if (count == 0) {
                      int_parent_value =
                          "(struct metal_interrupt *)&__metal_dt_" +
                          m.handle() + ".controller";
                    }
                  });

    /* Interrupt line */
    uint32_t irline = 0;
    n.maybe_tuple("interrupts", tuple_t<uint32_t>(), [&]() {},
                  [&](uint32_t ir) { irline = ir; });

    /* Define inline functions */
    if (count == 0) {
      control_base_func = create_inline_def(
          "control_base", "unsigned long",
          "(uintptr_t)i2c == (uintptr_t)&__metal_dt_" + n.handle(),
          platform_define(n, METAL_BASE_ADDRESS_LABEL),
          "struct metal_i2c *i2c");

      control_size_func = create_inline_def(
          "control_size", "unsigned long",
          "(uintptr_t)i2c == (uintptr_t)&__metal_dt_" + n.handle(),
          platform_define(n, METAL_SIZE_LABEL), "struct metal_i2c *i2c");

      num_interrupts_func = create_inline_def("num_interrupts", "int", "empty",
                                              "METAL_MAX_I2C0_INTERRUPTS",
                                              "struct metal_i2c *i2c");

      interrupt_parent_func =
          create_inline_def("interrupt_parent", "struct metal_interrupt *",
                            "empty", int_parent_value, "struct metal_i2c *i2c");

      clock_func = create_inline_def(
          "clock", "struct metal_clock *",
          "(uintptr_t)i2c == (uintptr_t)&__metal_dt_" + n.handle(), clock_value,
          "struct metal_i2c *i2c");

      pinmux_func = create_inline_def(
          "pinmux", "struct __metal_driver_sifive_gpio0 *",
          "(uintptr_t)i2c == (uintptr_t)&__metal_dt_" + n.handle(),
          pinmux_value, "struct metal_i2c *i2c");

      pinmux_output_selector_func = create_inline_def(
          "pinmux_output_selector", "unsigned long",
          "(uintptr_t)i2c == (uintptr_t)&__metal_dt_" + n.handle(),
          std::to_string(pinmux_dest), "struct metal_i2c *i2c");

      pinmux_source_selector_func = create_inline_def(
          "pinmux_source_selector", "unsigned long",
          "(uintptr_t)i2c == (uintptr_t)&__metal_dt_" + n.handle(),
          std::to_string(pinmux_source), "struct metal_i2c *i2c");

      interrupt_line_func = create_inline_def(
          "interrupt_line", "int",
          "(uintptr_t)i2c == (uintptr_t)&__metal_dt_" + n.handle(),
          std::to_string(irline), "struct metal_i2c *i2c");

    } else { /* count > 0 */
      add_inline_body(control_base_func,
                      "(uintptr_t)i2c == (uintptr_t)&__metal_dt_" + n.handle(),
                      platform_define(n, METAL_BASE_ADDRESS_LABEL));

      add_inline_body(control_size_func,
                      "(uintptr_t)i2c == (uintptr_t)&__metal_dt_" + n.handle(),
                      platform_define(n, METAL_SIZE_LABEL));

      add_inline_body(clock_func,
                      "(uintptr_t)i2c == (uintptr_t)&__metal_dt_" + n.handle(),
                      clock_value);

      add_inline_body(pinmux_func,
                      "(uintptr_t)i2c == (uintptr_t)&__metal_dt_" + n.handle(),
                      pinmux_value);

      add_inline_body(pinmux_output_selector_func,
                      "(uintptr_t)i2c == (uintptr_t)&__metal_dt_" + n.handle(),
                      std::to_string(pinmux_dest));

      add_inline_body(pinmux_source_selector_func,
                      "(uintptr_t)i2c == (uintptr_t)&__metal_dt_" + n.handle(),
                      std::to_string(pinmux_source));

      add_inline_body(interrupt_line_func,
                      "(uintptr_t)i2c == (uintptr_t)&__metal_dt_" + n.handle(),
                      std::to_string(irline));
    }

    count++;
  });

  os << "\n";
  os << "/* --------------------- sifive_i2c0 ------------ */\n";

  if (num_i2cs != 0) {
    add_inline_body(control_base_func, "else", "0");
    add_inline_body(control_size_func, "else", "0");
    add_inline_body(clock_func, "else", "NULL");
    add_inline_body(pinmux_func, "else", "NULL");
    add_inline_body(pinmux_output_selector_func, "else", "0");
    add_inline_body(pinmux_source_selector_func, "else", "0");
    add_inline_body(interrupt_line_func, "else", "0");

    emit_inline_def(control_base_func, "sifive_i2c0");
    delete control_base_func;
    emit_inline_def(control_size_func, "sifive_i2c0");
    delete control_size_func;
    emit_inline_def(clock_func, "sifive_i2c0");
    delete clock_func;
    emit_inline_def(pinmux_func, "sifive_i2c0");
    delete pinmux_func;
    emit_inline_def(pinmux_output_selector_func, "sifive_i2c0");
    delete pinmux_output_selector_func;
    emit_inline_def(pinmux_source_selector_func, "sifive_i2c0");
    delete pinmux_source_selector_func;
    emit_inline_def(num_interrupts_func, "sifive_i2c0");
    delete num_interrupts_func;
    emit_inline_def(interrupt_parent_func, "sifive_i2c0");
    delete interrupt_parent_func;
    emit_inline_def(interrupt_line_func, "sifive_i2c0");
    delete interrupt_line_func;
  }
  os << "\n";
}

void sifive_i2c0::declare_structs() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_struct_decl("sifive_i2c0", n); });
}

void sifive_i2c0::define_structs() {
  dtb.match(std::regex(compat_string), [&](node n) {
    emit_struct_begin("sifive_i2c0", n);
    emit_struct_field("i2c.vtable", "&__metal_driver_vtable_sifive_i2c0.i2c");
    emit_struct_end();
  });
}

void sifive_i2c0::create_handles() {
  emit_def("__METAL_DT_MAX_I2CS", std::to_string(num_i2cs));

  emit_struct_pointer_begin("sifive_i2c0", "__metal_i2c_table", "[]");
  if (num_i2cs) {
    int i = 0;
    dtb.match(std::regex(compat_string), [&](node n) {
      os << "\t\t\t\t\t&__metal_dt_" << n.handle();
      if ((i + 1) == num_i2cs) {
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
