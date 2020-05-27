/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <sifive_rtc0.h>

#include <regex>

sifive_rtc0::sifive_rtc0(std::ostream &os, const fdt &dtb)
    : Device(os, dtb, "sifive,(rtc0|aon0)") {
  num_rtcs = 0;
  dtb.match(std::regex(compat_string), [&](node n) { num_rtcs += 1; });
}

void sifive_rtc0::include_headers() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_include("sifive,rtc0"); });
}

void sifive_rtc0::declare_inlines() {
  Inline *func;
  std::list<Inline *> extern_inlines;
  int count = 0;

  dtb.match(std::regex(compat_string), [&](node n) {
    if (count == 0) {
      func = create_inline_dec("control_base", "unsigned long",
                               "const struct metal_rtc *const rtc");
      extern_inlines.push_back(func);

      func = create_inline_dec("control_size", "unsigned long",
                               "const struct metal_rtc *const rtc");
      extern_inlines.push_back(func);

      func = create_inline_dec("interrupt_parent", "struct metal_interrupt *",
                               "const struct metal_rtc *const rtc");
      extern_inlines.push_back(func);

      func = create_inline_dec("interrupt_line", "int",
                               "const struct metal_rtc *const rtc");
      extern_inlines.push_back(func);

      func = create_inline_dec("clock", "struct metal_clock *",
                               "const struct metal_rtc *const rtc");
      extern_inlines.push_back(func);
    }
    count++;
  });
  os << "\n";
  os << "/* --------------------- sifive_rtc0 ------------ */\n";
  while (!extern_inlines.empty()) {
    func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_dec(func, "sifive_rtc0");
    delete func;
  }
  os << "\n";
}

void sifive_rtc0::define_inlines() {
  Inline *base_func;
  Inline *size_func;
  Inline *int_parent_func;
  Inline *int_line_func;
  Inline *clock_func;

  int count = 0;

  dtb.match(std::regex(compat_string), [&](node n) {
    /* Interrupts */
    std::string int_parent_value = "NULL";
    n.maybe_tuple("interrupt-parent", tuple_t<node>(), [&]() {},
                  [&](node m) {
                    if (count == 0) {
                      int_parent_value =
                          "(struct metal_interrupt *)&__metal_dt_" +
                          m.handle() + ".controller";
                    }
                  });
    std::string int_line_value = "0";

    if (n.field_exists("interrupts")) {
      /* If the compatible string is "sifive,rtc0", then there should only be
       * one interrupt line. If the compatible string is "sifive,aon0", the
       * second interrupt line corresponds to the rtc interrupt. */
      int_line_value =
          std::to_string(n.get_fields<uint32_t>("interrupts").back());
    }

    std::string clock_value = "NULL";
    n.maybe_tuple("clocks", tuple_t<node>(), [&]() {},
                  [&](node m) {
                    clock_value = "(struct metal_clock *)&__metal_dt_" +
                                  m.handle() + ".clock";
                  });

    if (count == 0) {
      base_func = create_inline_def(
          "control_base", "unsigned long",
          "(uintptr_t)rtc == (uintptr_t)&__metal_dt_rtc_" + n.instance(),
          platform_define(n, METAL_BASE_ADDRESS_LABEL),
          "const struct metal_rtc *const rtc");

      size_func = create_inline_def(
          "control_size", "unsigned long",
          "(uintptr_t)rtc == (uintptr_t)&__metal_dt_rtc_" + n.instance(),
          platform_define(n, METAL_SIZE_LABEL),
          "const struct metal_rtc *const rtc");

      int_parent_func = create_inline_def(
          "interrupt_parent", "struct metal_interrupt *",
          "(uintptr_t)rtc == (uintptr_t)&__metal_dt_rtc_" + n.instance(),
          int_parent_value, "const struct metal_rtc *const rtc");

      int_line_func = create_inline_def(
          "interrupt_line", "int",
          "(uintptr_t)rtc == (uintptr_t)&__metal_dt_rtc_" + n.instance(),
          int_line_value, "const struct metal_rtc *const rtc");

      clock_func = create_inline_def(
          "clock", "struct metal_clock *",
          "(uintptr_t)rtc == (uintptr_t)&__metal_dt_rtc_" + n.instance(),
          clock_value, "const struct metal_rtc *const rtc");
    } else { /* count > 0 */
      add_inline_body(base_func,
                      "(uintptr_t)rtc == (uintptr_t)&__metal_dt_rtc_" +
                          n.instance(),
                      platform_define(n, METAL_BASE_ADDRESS_LABEL));

      add_inline_body(size_func,
                      "(uintptr_t)rtc == (uintptr_t)&__metal_dt_rtc_" +
                          n.instance(),
                      platform_define(n, METAL_SIZE_LABEL));

      add_inline_body(int_parent_func,
                      "(uintptr_t)rtc == (uintptr_t)&__metal_dt_rtc_" +
                          n.instance(),
                      int_parent_value);

      add_inline_body(int_line_func,
                      "(uintptr_t)rtc == (uintptr_t)&__metal_dt_rtc_" +
                          n.instance(),
                      int_line_value);

      add_inline_body(clock_func,
                      "(uintptr_t)rtc == (uintptr_t)&__metal_dt_rtc_" +
                          n.instance(),
                      clock_value);
    }

    count += 1;
  });

  os << "\n";
  os << "/* --------------------- sifive_rtc0 ------------ */\n";

  if (num_rtcs != 0) {
    add_inline_body(base_func, "else", "0");
    add_inline_body(size_func, "else", "0");
    add_inline_body(int_parent_func, "else", "0");
    add_inline_body(int_line_func, "else", "0");
    add_inline_body(clock_func, "else", "0");

    emit_inline_def(base_func, "sifive_rtc0");
    delete base_func;
    emit_inline_def(size_func, "sifive_rtc0");
    delete size_func;
    emit_inline_def(int_parent_func, "sifive_rtc0");
    delete int_parent_func;
    emit_inline_def(int_line_func, "sifive_rtc0");
    delete int_line_func;
    emit_inline_def(clock_func, "sifive_rtc0");
    delete clock_func;
  }
  os << "\n";
}

void sifive_rtc0::declare_structs() {
  dtb.match(std::regex(compat_string), [&](node n) {
    emit_comment(n);
    os << "extern struct __metal_driver_sifive_rtc0 __metal_dt_rtc_"
       << n.instance() << ";\n\n";
  });
}

void sifive_rtc0::define_structs() {
  dtb.match(std::regex(compat_string), [&](node n) {
    emit_comment(n);
    os << "struct __metal_driver_sifive_rtc0 __metal_dt_rtc_" << n.instance()
       << " = {\n";

    emit_struct_field("rtc.vtable", "&__metal_driver_vtable_sifive_rtc0.rtc");

    emit_struct_end();
  });
}

void sifive_rtc0::create_handles() {
  emit_def("__METAL_DT_MAX_RTCS", std::to_string(num_rtcs));

  emit_struct_pointer_begin("sifive_rtc0", "__metal_rtc_table", "[]");
  if (num_rtcs) {
    int i = 0;
    dtb.match(std::regex(compat_string), [&](node n) {
      os << "\t\t\t\t\t&__metal_dt_rtc_" << n.instance();

      if ((i + 1) == num_rtcs) {
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
