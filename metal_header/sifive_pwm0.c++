/* Copyright 2020 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <sifive_pwm0.h>

sifive_pwm0::sifive_pwm0(std::ostream &os, const fdt &dtb)
    : Device(os, dtb, "sifive,pwm0") {
  /* Count the number of PWMs */
  num_pwms = 0;
  dtb.match(std::regex(compat_string), [&](node n) { num_pwms += 1; });
}

void sifive_pwm0::create_defines() {
  dtb.match(std::regex(compat_string), [&](node n) {
    uint32_t num_interrupts = n.get_fields_count<uint32_t>("interrupts");
    emit_def("__METAL_" + n.handle_cap() + "_INTERRUPTS",
             std::to_string(num_interrupts));

    if (num_interrupts > max_interrupts) {
      max_interrupts = num_interrupts;
    }

    uint32_t ncmp = 1;
    if (n.field_exists("sifive,ncomparators")) {
      ncmp = n.get_fields<uint32_t>("sifive,ncomparators").back();
    }
    if (ncmp > max_ncmp) {
      max_ncmp = ncmp;
    }
  });

  emit_def("METAL_MAX_PWM0_INTERRUPTS", std::to_string(max_interrupts));
  emit_def("METAL_MAX_PWM0_NCMP", std::to_string(max_ncmp));
}

void sifive_pwm0::include_headers() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_include(compat_string); });
}

void sifive_pwm0::declare_inlines() {
  Inline *func;
  std::list<Inline *> extern_inlines;
  int count = 0;

  dtb.match(std::regex(compat_string), [&](node n) {
    if (count == 0) {
      func = create_inline_dec("control_base", "unsigned long",
                               "struct metal_pwm *pwm");
      extern_inlines.push_back(func);

      func = create_inline_dec("control_size", "unsigned long",
                               "struct metal_pwm *pwm");
      extern_inlines.push_back(func);

      func =
          create_inline_dec("num_interrupts", "int", "struct metal_pwm *pwm");
      extern_inlines.push_back(func);

      func = create_inline_dec("interrupt_parent", "struct metal_interrupt *",
                               "struct metal_pwm *pwm");
      extern_inlines.push_back(func);

      func = create_inline_dec("interrupt_lines", "int",
                               "struct metal_pwm *pwm", "int idx");
      extern_inlines.push_back(func);

      func = create_inline_dec("clock", "struct metal_clock *",
                               "struct metal_pwm *pwm");
      extern_inlines.push_back(func);

      func = create_inline_dec("pinmux", "struct __metal_driver_sifive_gpio0 *",
                               "struct metal_pwm *pwm");
      extern_inlines.push_back(func);

      func = create_inline_dec("pinmux_output_selector", "unsigned long",
                               "struct metal_pwm *pwm");
      extern_inlines.push_back(func);

      func = create_inline_dec("pinmux_source_selector", "unsigned long",
                               "struct metal_pwm *pwm");
      extern_inlines.push_back(func);

      func = create_inline_dec("compare_width", "int", "struct metal_pwm *pwm");
      extern_inlines.push_back(func);

      func =
          create_inline_dec("comparator_count", "int", "struct metal_pwm *pwm");
      extern_inlines.push_back(func);
    }
    count++;
  });
  os << "\n";
  os << "/* --------------------- sifive_pwm0 ------------ */\n";
  while (!extern_inlines.empty()) {
    func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_dec(func, "sifive_pwm0");
    delete func;
  }
  os << "\n";
}

void sifive_pwm0::define_inlines() {

  Inline *control_base_func;
  Inline *control_size_func;
  Inline *num_interrupts_func;
  Inline *clock_func;
  Inline *pinmux_func;
  Inline *pinmux_output_selector_func;
  Inline *pinmux_source_selector_func;
  Inline *interrupt_parent_func;
  Inline *interrupt_line_func;
  Inline *compare_width_func;
  Inline *comparator_count_func;
  int count = 0;

  dtb.match(std::regex(compat_string), [&](node n) {
    /* Clock driving the pwm peripheral */
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
    if (n.field_exists("pinmux")) {
      n.maybe_tuple("pinmux", tuple_t<node, uint32_t, uint32_t>(), [&]() {},
                    [&](node m, uint32_t dest, uint32_t source) {
                      pinmux_value =
                          "(struct __metal_driver_sifive_gpio0 *)&__metal_dt_" +
                          m.handle();
                      pinmux_dest = dest;
                      pinmux_source = source;
                    });
    }

    /* Comparator count and width */
    uint32_t ncmp = 1;
    if (n.field_exists("sifive,ncomparators")) {
      ncmp = n.get_fields<uint32_t>("sifive,ncomparators").back();
    }
    uint32_t cmpwidth = 16;
    if (n.field_exists("sifive,comparator-widthbits")) {
      cmpwidth = n.get_fields<uint32_t>("sifive,comparator-widthbits").back();
    }

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

    /* Interrupt lines */
    n.maybe_tuple_index(
        "interrupts", tuple_t<uint32_t>(),
        [&]() {
          if (count == 0) {
            interrupt_line_func =
                create_inline_def("interrupt_lines", "int", "empty", "0",
                                  "struct metal_pwm *pwm", "int idx");
          }
        },

        [&](int i, uint32_t irline) {
          uint32_t num_interrupts = n.get_fields_count<uint32_t>("interrupts");
          if ((count == 0) && (i == 0)) {
            interrupt_line_func = create_inline_def(
                "interrupt_lines", "int",
                "((uintptr_t)pwm == (uintptr_t)&__metal_dt_" + n.handle() +
                    ") && (" + "idx == " + std::to_string(i) + ")",
                std::to_string(irline), "struct metal_pwm *pwm", "int idx");
          } else if (((count + 1) == num_pwms) && ((i + 1) == num_interrupts)) {
            add_inline_body(interrupt_line_func,
                            "(((uintptr_t)pwm == (uintptr_t)&__metal_dt_" +
                                n.handle() + ") && (" +
                                "idx == " + std::to_string(i) + "))",
                            std::to_string(irline));
            add_inline_body(interrupt_line_func, "else", "0");
          } else {
            add_inline_body(interrupt_line_func,
                            "(((uintptr_t)pwm == (uintptr_t)&__metal_dt_" +
                                n.handle() + ") && (" +
                                "idx == " + std::to_string(i) + "))",
                            std::to_string(irline));
          }
        });

    /* Define inline functions */
    if (count == 0) {
      control_base_func = create_inline_def(
          "control_base", "unsigned long",
          "(uintptr_t)pwm == (uintptr_t)&__metal_dt_" + n.handle(),
          platform_define(n, METAL_BASE_ADDRESS_LABEL),
          "struct metal_pwm *pwm");

      control_size_func = create_inline_def(
          "control_size", "unsigned long",
          "(uintptr_t)pwm == (uintptr_t)&__metal_dt_" + n.handle(),
          platform_define(n, METAL_SIZE_LABEL), "struct metal_pwm *pwm");

      num_interrupts_func = create_inline_def(
          "num_interrupts", "int",
          "(uintptr_t)pwm == (uintptr_t)&__metal_dt_" + n.handle(),
          "__METAL_" + n.handle_cap() + "_INTERRUPTS", "struct metal_pwm *pwm");

      interrupt_parent_func =
          create_inline_def("interrupt_parent", "struct metal_interrupt *",
                            "empty", int_parent_value, "struct metal_pwm *pwm");

      clock_func = create_inline_def(
          "clock", "struct metal_clock *",
          "(uintptr_t)pwm == (uintptr_t)&__metal_dt_" + n.handle(), clock_value,
          "struct metal_pwm *pwm");

      pinmux_func = create_inline_def(
          "pinmux", "struct __metal_driver_sifive_gpio0 *",
          "(uintptr_t)pwm == (uintptr_t)&__metal_dt_" + n.handle(),
          pinmux_value, "struct metal_pwm *pwm");

      pinmux_output_selector_func = create_inline_def(
          "pinmux_output_selector", "unsigned long",
          "(uintptr_t)pwm == (uintptr_t)&__metal_dt_" + n.handle(),
          std::to_string(pinmux_dest), "struct metal_pwm *pwm");

      pinmux_source_selector_func = create_inline_def(
          "pinmux_source_selector", "unsigned long",
          "(uintptr_t)pwm == (uintptr_t)&__metal_dt_" + n.handle(),
          std::to_string(pinmux_source), "struct metal_pwm *pwm");

      compare_width_func = create_inline_def(
          "compare_width", "int",
          "(uintptr_t)pwm == (uintptr_t)&__metal_dt_" + n.handle(),
          std::to_string(cmpwidth), "struct metal_pwm *pwm");

      comparator_count_func = create_inline_def(
          "comparator_count", "int",
          "(uintptr_t)pwm == (uintptr_t)&__metal_dt_" + n.handle(),
          std::to_string(ncmp), "struct metal_pwm *pwm");

    } else { /* count > 0 */
      add_inline_body(control_base_func,
                      "(uintptr_t)pwm == (uintptr_t)&__metal_dt_" + n.handle(),
                      platform_define(n, METAL_BASE_ADDRESS_LABEL));

      add_inline_body(control_size_func,
                      "(uintptr_t)pwm == (uintptr_t)&__metal_dt_" + n.handle(),
                      platform_define(n, METAL_SIZE_LABEL));

      add_inline_body(num_interrupts_func,
                      "(uintptr_t)pwm == (uintptr_t)&__metal_dt_" + n.handle(),
                      "__METAL_" + n.handle_cap() + "_INTERRUPTS");

      add_inline_body(clock_func,
                      "(uintptr_t)pwm == (uintptr_t)&__metal_dt_" + n.handle(),
                      clock_value);

      add_inline_body(pinmux_func,
                      "(uintptr_t)pwm == (uintptr_t)&__metal_dt_" + n.handle(),
                      pinmux_value);

      add_inline_body(pinmux_output_selector_func,
                      "(uintptr_t)pwm == (uintptr_t)&__metal_dt_" + n.handle(),
                      std::to_string(pinmux_dest));

      add_inline_body(pinmux_source_selector_func,
                      "(uintptr_t)pwm == (uintptr_t)&__metal_dt_" + n.handle(),
                      std::to_string(pinmux_source));

      add_inline_body(compare_width_func,
                      "(uintptr_t)pwm == (uintptr_t)&__metal_dt_" + n.handle(),
                      std::to_string(cmpwidth));

      add_inline_body(comparator_count_func,
                      "(uintptr_t)pwm == (uintptr_t)&__metal_dt_" + n.handle(),
                      std::to_string(ncmp));
    }

    count++;
  });

  os << "\n";
  os << "/* --------------------- sifive_pwm0 ------------ */\n";

  if (num_pwms != 0) {
    add_inline_body(control_base_func, "else", "0");
    add_inline_body(control_size_func, "else", "0");
    add_inline_body(clock_func, "else", "NULL");
    add_inline_body(pinmux_func, "else", "NULL");
    add_inline_body(pinmux_output_selector_func, "else", "0");
    add_inline_body(pinmux_source_selector_func, "else", "0");
    add_inline_body(compare_width_func, "else", "0");
    add_inline_body(comparator_count_func, "else", "0");
    add_inline_body(num_interrupts_func, "else", "0");

    emit_inline_def(control_base_func, "sifive_pwm0");
    delete control_base_func;
    emit_inline_def(control_size_func, "sifive_pwm0");
    delete control_size_func;
    emit_inline_def(clock_func, "sifive_pwm0");
    delete clock_func;
    emit_inline_def(pinmux_func, "sifive_pwm0");
    delete pinmux_func;
    emit_inline_def(pinmux_output_selector_func, "sifive_pwm0");
    delete pinmux_output_selector_func;
    emit_inline_def(pinmux_source_selector_func, "sifive_pwm0");
    delete pinmux_source_selector_func;
    emit_inline_def(num_interrupts_func, "sifive_pwm0");
    delete num_interrupts_func;
    emit_inline_def(interrupt_parent_func, "sifive_pwm0");
    delete interrupt_parent_func;
    emit_inline_def(interrupt_line_func, "sifive_pwm0");
    delete interrupt_line_func;
    emit_inline_def(compare_width_func, "sifive_pwm0");
    delete compare_width_func;
    emit_inline_def(comparator_count_func, "sifive_pwm0");
    delete comparator_count_func;
  }
  os << "\n";
}

void sifive_pwm0::declare_structs() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_struct_decl("sifive_pwm0", n); });
}

void sifive_pwm0::define_structs() {
  dtb.match(std::regex(compat_string), [&](node n) {
    emit_struct_begin("sifive_pwm0", n);
    emit_struct_field("pwm.vtable", "&__metal_driver_vtable_sifive_pwm0.pwm");
    emit_struct_end();
  });
}

void sifive_pwm0::create_handles() {
  emit_def("__METAL_DT_MAX_PWMS", std::to_string(num_pwms));

  emit_struct_pointer_begin("sifive_pwm0", "__metal_pwm_table", "[]");
  if (num_pwms) {
    int i = 0;
    dtb.match(std::regex(compat_string), [&](node n) {
      os << "\t\t\t\t\t&__metal_dt_" << n.handle();
      if ((i + 1) == num_pwms) {
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
