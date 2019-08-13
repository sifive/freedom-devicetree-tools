/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <fixed_factor_clock.h>

fixed_factor_clock::fixed_factor_clock(std::ostream &os, const fdt &dtb)
    : Device(os, dtb, "fixed-factor-clock") {
  /* Count the number of Fixed-Factor-Clocks */
  num_clocks = 0;
  dtb.match(std::regex(compat_string), [&](node n) { num_clocks += 1; });
}

void fixed_factor_clock::include_headers() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_include(compat_string); });
}

void fixed_factor_clock::declare_inlines() {
  Inline *func;
  std::list<Inline *> extern_inlines;
  int count = 0;

  dtb.match(std::regex(compat_string), [&](node n) {
    if (count == 0) {
      func = create_inline_dec("parent", "struct metal_clock *",
                               "const struct metal_clock *clock");
      extern_inlines.push_back(func);

      func = create_inline_dec("mult", "unsigned long",
                               "const struct metal_clock *clock");
      extern_inlines.push_back(func);
      func = create_inline_dec("div", "unsigned long",
                               "const struct metal_clock *clock");
      extern_inlines.push_back(func);
    }
    count++;
  });
  os << "\n";
  os << "/* --------------------- fixed_factor_clock ------------ */\n";
  while (!extern_inlines.empty()) {
    func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_dec(func, "fixed_factor_clock");
    delete func;
  }
  os << "\n";
}

void fixed_factor_clock::define_inlines() {
  Inline *clock_func;
  Inline *mult_func;
  Inline *div_func;
  std::list<Inline *> extern_inlines;

  int count = 0;

  dtb.match(std::regex(compat_string), [&](node n) {
    std::string clock_value = "NULL";
    n.maybe_tuple_index("clocks", tuple_t<node>(), [&]() {},
                        [&](int i, node m) {
                          if (count == 0) {
                            clock_value = "(struct metal_clock *)&__metal_dt_" +
                                          m.handle() + ".clock";
                          }
                        });

    if (count == 0) {
      clock_func = create_inline_def(
          "parent", "struct metal_clock *",
          "(uintptr_t)clock == (uintptr_t)&__metal_dt_" + n.handle(),
          clock_value, "const struct metal_clock *clock");

      mult_func = create_inline_def(
          "mult", "unsigned long",
          "(uintptr_t)clock == (uintptr_t)&__metal_dt_" + n.handle(),
          platform_define(n, METAL_CLOCK_MULT_LABEL),
          "const struct metal_clock *clock");

      div_func = create_inline_def(
          "div", "unsigned long",
          "(uintptr_t)clock == (uintptr_t)&__metal_dt_" + n.handle(),
          platform_define(n, METAL_CLOCK_DIV_LABEL),
          "const struct metal_clock *clock");
    }
    if (count > 0) {
      add_inline_body(
          mult_func, "(uintptr_t)clock == (uintptr_t)&__metal_dt_" + n.handle(),
          platform_define(n, METAL_CLOCK_MULT_LABEL));
      add_inline_body(
          div_func, "(uintptr_t)clock == (uintptr_t)&__metal_dt_" + n.handle(),
          platform_define(n, METAL_CLOCK_DIV_LABEL));
    }
    if ((count + 1) == num_clocks) {
      add_inline_body(clock_func, "else", "NULL");
      add_inline_body(mult_func, "else", "0");
      add_inline_body(div_func, "else", "0");
    }

    count++;
  });

  if (num_clocks != 0) {
    extern_inlines.push_back(clock_func);
    extern_inlines.push_back(mult_func);
    extern_inlines.push_back(div_func);
  }

  os << "\n";
  os << "/* --------------------- fixed_factor_clock ------------ */\n";
  while (!extern_inlines.empty()) {
    Inline *func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_def(func, "fixed_factor_clock");
    delete func;
  }
  os << "\n";
}

void fixed_factor_clock::declare_structs() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_struct_decl("fixed_factor_clock", n); });
}

void fixed_factor_clock::define_structs() {
  dtb.match(std::regex(compat_string), [&](node n) {
    emit_struct_begin("fixed_factor_clock", n);
    emit_struct_field("clock.vtable",
                      "&__metal_driver_vtable_fixed_factor_clock.clock");
    emit_struct_end();
  });
}
