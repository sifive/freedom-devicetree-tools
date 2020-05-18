/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

/****************************************

  DEVICE TEMPLATE

  This file is a template for adding support for new hardware to Freedom Metal.

  To add a new device, create a copy of this file and rename all instances of
  "sifive_buserror0" to the name of the device you're adding. Make sure you
  remember to change the #ifndef at the top too!

****************************************/

#include <sifive_buserror0.h>

#include <regex>

sifive_buserror0::sifive_buserror0(std::ostream &os, const fdt &dtb)
    : Device(os, dtb, "sifive,buserror0") {
  num_error_units = 0;

  dtb.match(std::regex(compat_string), [&](node n) { num_error_units += 1; });
  ;
}

void sifive_buserror0::include_headers() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_include(compat_string); });
}

void sifive_buserror0::declare_inlines() {
  Inline *func;
  std::list<Inline *> extern_inlines;
  int count = 0;

  dtb.match(std::regex(compat_string), [&](node n) {
    if (count == 0) {
      func = create_inline_dec("control_base", "uintptr_t",
                               "const struct metal_buserror *beu");
      extern_inlines.push_back(func);

      func = create_inline_dec("interrupt_parent", "struct metal_interrupt *",
                               "const struct metal_buserror *beu");
      extern_inlines.push_back(func);

      func = create_inline_dec("interrupt_id", "int",
                               "const struct metal_buserror *beu");
      extern_inlines.push_back(func);
    }
    count++;
  });
  os << "\n";
  os << "/* --------------------- sifive_buserror0 ------------ */\n";
  while (!extern_inlines.empty()) {
    func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_dec(func, "sifive_buserror0");
    delete func;
  }
  os << "\n";
}

void sifive_buserror0::define_inlines() {
  Inline *base_func;
  Inline *int_parent_func;
  Inline *int_id_func;
  std::list<Inline *> extern_inlines;

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
    std::string int_id_value = "0";

    if (n.field_exists("interrupts")) {
      /* If the compatible string is "sifive,wdog0", then there should only be
       * one interrupt line. If the compatible string is "sifive,aon0", the
       * first interrupt line corresponds to the watchdog interrupt. */
      int_id_value = std::to_string(n.get_fields<uint32_t>("interrupts").at(0));
    }

    if (count == 0) {
      base_func = create_inline_def(
          "control_base", "uintptr_t",
          "(uintptr_t)buserror == (uintptr_t)&__metal_dt_" + n.handle(),
          platform_define(n, METAL_BASE_ADDRESS_LABEL),
          "const struct metal_buserror *buserror");
      extern_inlines.push_back(base_func);

      int_parent_func = create_inline_def(
          "interrupt_parent", "struct metal_interrupt *",
          "(uintptr_t)buserror == (uintptr_t)&__metal_dt_" + n.handle(),
          int_parent_value, "const struct metal_buserror *buserror");
      extern_inlines.push_back(int_parent_func);

      int_id_func = create_inline_def(
          "interrupt_id", "int",
          "(uintptr_t)buserror == (uintptr_t)&__metal_dt_" + n.handle(),
          int_id_value, "const struct metal_buserror *buserror");
      extern_inlines.push_back(int_id_func);
    }
    if (count > 0) {
      add_inline_body(base_func,
                      "(uintptr_t)buserror == (uintptr_t)&__metal_dt_" +
                          n.handle(),
                      platform_define(n, METAL_BASE_ADDRESS_LABEL));
      add_inline_body(int_parent_func,
                      "(uintptr_t)buserror == (uintptr_t)&__metal_dt_" +
                          n.handle(),
                      int_parent_value);
      add_inline_body(int_id_func,
                      "(uintptr_t)buserror == (uintptr_t)&__metal_dt_" +
                          n.handle(),
                      int_id_value);
    }
    if ((count + 1) == num_error_units) {
      add_inline_body(base_func, "else", "0");
      add_inline_body(int_parent_func, "else", "NULL");
      add_inline_body(int_id_func, "else", "0");
    }
    count++;
  });

  os << "\n";
  os << "/* --------------------- sifive_buserror0 ------------ */\n";
  while (!extern_inlines.empty()) {
    Inline *func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_def(func, "sifive_buserror0");
    delete func;
  }
  os << "\n";
}

/* This method declares the device driver struct */
void sifive_buserror0::declare_structs() {
  dtb.match(std::regex(compat_string), [&](node n) {
    emit_comment(n);
    os << "extern struct metal_buserror"
          " __metal_dt_"
       << n.handle() << ";\n\n";
  });
}

/* This method defines the device driver struct, and is also where you
 * should output the values for the struct fields */
void sifive_buserror0::define_structs() {
  dtb.match(std::regex(compat_string), [&](node n) {
    emit_comment(n);
    os << "struct metal_buserror"
          " __metal_dt_"
       << n.handle() << " = {\n";

    /* Emit struct fields here */

    emit_struct_end();
  });
}
