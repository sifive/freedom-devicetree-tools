/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <riscv_dma0.h>

riscv_dma0::riscv_dma0(std::ostream &os, const fdt &dtb)
    : Device(os, dtb, "riscv,dma0") {}

void riscv_dma0::create_defines() {
  dtb.match(std::regex(compat_string), [&](node n) {
    num_dmas += 1;

    uint32_t num_interrupts = n.get_fields_count<uint32_t>("interrupts");

    emit_def("__METAL_" + n.handle_cap() + "_INTERRUPTS",
             std::to_string(num_interrupts));

    if (num_interrupts > max_interrupts) {
      max_interrupts = num_interrupts;
    }
  });

  emit_def("METAL_MAX_DMA_INTERRUPTS", std::to_string(max_interrupts));
}

void riscv_dma0::include_headers() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_include(compat_string); });
}

void riscv_dma0::declare_inlines() {
  Inline *func;
  std::list<Inline *> extern_inlines;
  int count = 0;

  dtb.match(std::regex(compat_string), [&](node n) {
    if (count == 0) {
      func = create_inline_dec("control_base", "unsigned long",
                               "struct metal_dma *dma");
      extern_inlines.push_back(func);

      func = create_inline_dec("control_size", "unsigned long",
                               "struct metal_dma *dma");
      extern_inlines.push_back(func);

      func =
          create_inline_dec("num_interrupts", "int", "struct metal_dma *dma");
      extern_inlines.push_back(func);

      func = create_inline_dec("interrupt_parent", "struct metal_interrupt *",
                               "struct metal_dma *dma");
      extern_inlines.push_back(func);

      func =
          create_inline_dec("interrupt_line", "int", "struct metal_dma *dma");
      extern_inlines.push_back(func);

      func = create_inline_dec("clock", "struct metal_clock *",
                               "struct metal_dma *dma");
      extern_inlines.push_back(func);

      func = create_inline_dec("pinmux", "struct __metal_driver_sifive_gpio0 *",
                               "struct metal_dma *dma");
      extern_inlines.push_back(func);

      func = create_inline_dec("pinmux_output_selector", "unsigned long",
                               "struct metal_dma *dma");
      extern_inlines.push_back(func);

      func = create_inline_dec("pinmux_source_selector", "unsigned long",
                               "struct metal_dma *dma");
      extern_inlines.push_back(func);
    }
    count++;
  });
  os << "\n";
  os << "/* --------------------- riscv_dma0 ------------ */\n";
  while (!extern_inlines.empty()) {
    func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_dec(func, "riscv_dma0");
    delete func;
  }
  os << "\n";
}

void riscv_dma0::define_inlines() {
  Inline *base_func;
  Inline *size_func;
  Inline *num_int_func;
  Inline *int_parent_func;
  Inline *int_line_func;
  Inline *clock_func;
  Inline *pinmux_func;
  Inline *pinmux_out_func;
  Inline *pinmux_source_func;

  int count = 0;

  dtb.match(std::regex(compat_string), [&](node n) {
    /* Interrupts */
    std::string int_parent_value = "NULL";
    n.maybe_tuple("interrupt-parent", tuple_t<node>(), [&]() {},
                  [&](node m) {
                    int_parent_value =
                        "(struct metal_interrupt *)&__metal_dt_" + m.handle() +
                        ".controller";
                  });
    std::string int_line_value = "0";
    n.maybe_tuple(
        "interrupts", tuple_t<uint32_t>(), [&]() {},
        [&](uint32_t irline) { int_line_value = std::to_string(irline); });

    /* Clock driving the DMA peripheral */
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

    if (count == 0) {
      base_func = create_inline_def(
          "control_base", "unsigned long",
          "(uintptr_t)dma == (uintptr_t)&__metal_dt_" + n.handle(),
          platform_define(n, METAL_BASE_ADDRESS_LABEL),
          "struct metal_dma *dma");

      size_func = create_inline_def(
          "control_size", "unsigned long",
          "(uintptr_t)dma == (uintptr_t)&__metal_dt_" + n.handle(),
          platform_define(n, METAL_SIZE_LABEL), "struct metal_dma *dma");

      num_int_func = create_inline_def(
          "num_interrupts", "int",
          "(uintptr_t)dma == (uintptr_t)&__metal_dt_" + n.handle(),
          "METAL_MAX_DMA_INTERRUPTS", "struct metal_dma *dma");

      int_parent_func = create_inline_def(
          "interrupt_parent", "struct metal_interrupt *",
          "(uintptr_t)dma == (uintptr_t)&__metal_dt_" + n.handle(),
          int_parent_value, "struct metal_dma *dma");

      int_line_func = create_inline_def(
          "interrupt_line", "int",
          "(uintptr_t)dma == (uintptr_t)&__metal_dt_" + n.handle(),
          int_line_value, "struct metal_dma *dma");

      clock_func = create_inline_def(
          "clock", "struct metal_clock *",
          "(uintptr_t)dma == (uintptr_t)&__metal_dt_" + n.handle(),
          clock_value, "struct metal_dma *dma");

      pinmux_func = create_inline_def(
          "pinmux", "struct __metal_driver_sifive_gpio0 *",
          "(uintptr_t)dma == (uintptr_t)&__metal_dt_" + n.handle(),
          pinmux_value, "struct metal_dma *dma");

      pinmux_out_func = create_inline_def(
          "pinmux_output_selector", "unsigned long",
          "(uintptr_t)dma == (uintptr_t)&__metal_dt_" + n.handle(),
          std::to_string(pinmux_dest), "struct metal_dma *dma");

      pinmux_source_func = create_inline_def(
          "pinmux_source_selector", "unsigned long",
          "(uintptr_t)dma == (uintptr_t)&__metal_dt_" + n.handle(),
          std::to_string(pinmux_source), "struct metal_dma *dma");
    } else { /* count > 0 */
      add_inline_body(base_func,
                      "(uintptr_t)dma == (uintptr_t)&__metal_dt_" + n.handle(),
                      platform_define(n, METAL_BASE_ADDRESS_LABEL));

      add_inline_body(size_func,
                      "(uintptr_t)dma == (uintptr_t)&__metal_dt_" + n.handle(),
                      platform_define(n, METAL_SIZE_LABEL));

      add_inline_body(num_int_func,
                      "(uintptr_t)dma == (uintptr_t)&__metal_dt_" + n.handle(),
                      "METAL_MAX_DMA_INTERRUPTS");

      add_inline_body(int_parent_func,
                      "(uintptr_t)dma == (uintptr_t)&__metal_dt_" + n.handle(),
                      int_parent_value);

      add_inline_body(int_line_func,
                      "(uintptr_t)dma == (uintptr_t)&__metal_dt_" + n.handle(),
                      int_line_value);

      add_inline_body(clock_func,
                      "(uintptr_t)dma == (uintptr_t)&__metal_dt_" + n.handle(),
                      clock_value);

      add_inline_body(pinmux_func,
                      "(uintptr_t)dma == (uintptr_t)&__metal_dt_" + n.handle(),
                      pinmux_value);

      add_inline_body(pinmux_out_func,
                      "(uintptr_t)dma == (uintptr_t)&__metal_dt_" + n.handle(),
                      std::to_string(pinmux_dest));

      add_inline_body(pinmux_source_func,
                      "(uintptr_t)dma == (uintptr_t)&__metal_dt_" + n.handle(),
                      std::to_string(pinmux_source));
    };

    count += 1;
  });

  os << "\n";
  os << "/* --------------------- riscv_dma0 ------------ */\n";

  if (num_dmas != 0) {
    add_inline_body(base_func, "else", "0");
    add_inline_body(size_func, "else", "0");
    add_inline_body(num_int_func, "else", "0");
    add_inline_body(int_parent_func, "else", "0");
    add_inline_body(int_line_func, "else", "0");
    add_inline_body(clock_func, "else", "0");
    add_inline_body(pinmux_func, "else", "0");
    add_inline_body(pinmux_out_func, "else", "0");
    add_inline_body(pinmux_source_func, "else", "0");

    emit_inline_def(base_func, "riscv_dma0");
    delete base_func;
    emit_inline_def(size_func, "riscv_dma0");
    delete size_func;
    emit_inline_def(num_int_func, "riscv_dma0");
    delete num_int_func;
    emit_inline_def(int_parent_func, "riscv_dma0");
    delete int_parent_func;
    emit_inline_def(int_line_func, "riscv_dma0");
    delete int_line_func;
    emit_inline_def(clock_func, "riscv_dma0");
    delete clock_func;
    emit_inline_def(pinmux_func, "riscv_dma0");
    delete pinmux_func;
    emit_inline_def(pinmux_out_func, "riscv_dma0");
    delete pinmux_out_func;
    emit_inline_def(pinmux_source_func, "riscv_dma0");
    delete pinmux_source_func;
  }
  os << "\n";
}

void riscv_dma0::declare_structs() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_struct_decl("riscv_dma0", n); });
}

void riscv_dma0::define_structs() {
  dtb.match(std::regex(compat_string), [&](node n) {
    emit_struct_begin("riscv_dma0", n);

    emit_struct_field("dma.vtable",
                      "&__metal_driver_vtable_riscv_dma0.dma");

    emit_struct_end();
  });
}

void riscv_dma0::create_handles() {
  emit_def("__METAL_DT_MAX_DMAS", std::to_string(num_dmas));

  emit_struct_pointer_begin("riscv_dma0", "__metal_dma_table", "[]");
  if (num_dmas) {
    int i = 0;
    dtb.match(std::regex(compat_string), [&](node n) {
      os << "\t\t\t\t\t&__metal_dt_" << n.handle();

      if ((i + 1) == num_dmas) {
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
