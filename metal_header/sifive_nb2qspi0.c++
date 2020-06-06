/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <sifive_nb2qspi0.h>

sifive_nb2qspi0::sifive_nb2qspi0(std::ostream &os, const fdt &dtb)
  : Device(os, dtb, "sifive,nb2qspi0")
{
  /* Count the number of QSPIs */
  num_qspis = 0;
  dtb.match(
    std::regex(compat_string),
    [&](node n) {
      num_qspis += 1;
    });
}

void sifive_nb2qspi0::create_defines(){
	dtb.match(
	std::regex(compat_string),
	[&](node n) {
	if (n.field_exists("axi-base-addr")) {
		auto regs = n.get_fields<std::tuple<uint32_t, uint32_t>>("axi-base-addr");
		if (!regs.empty()) {
			uint32_t base = std::get<0>(regs.front());
			uint32_t size = std::get<1>(regs.front());
			emit_def("METAL_QSPI_AXI_BASE_ADDR", std::to_string(base));
			emit_def("METAL_QSPI_AXI_SIZE", std::to_string(size));
		}
	}
	});
}

void sifive_nb2qspi0::include_headers()
{
  dtb.match(
    std::regex(compat_string),
    [&](node n) {
      emit_include(compat_string);
    });
}

void sifive_nb2qspi0::declare_inlines()
{
  Inline* func;
  std::list<Inline *> extern_inlines;
  int count = 0;
  
  dtb.match(
    std::regex(compat_string),
    [&](node n) {
      if (count == 0) {
        func = create_inline_dec("control_base",
                                 "unsigned long",
                                 "struct metal_qspi *qspi");
        extern_inlines.push_back(func);

        func = create_inline_dec("control_size",
                                 "unsigned long",
                                 "struct metal_qspi *qspi");
        extern_inlines.push_back(func);

        func = create_inline_dec("pinmux",
                                 "struct __metal_driver_sifive_nb2gpio0 *",
                                 "struct metal_qspi *qspi");
        extern_inlines.push_back(func);

        func = create_inline_dec("pinmux_output_selector",
                                 "unsigned long",
                                 "struct metal_qspi *qspi");
        extern_inlines.push_back(func);

        func = create_inline_dec("pinmux_source_selector",
                                 "unsigned long",
                                 "struct metal_qspi *qspi");
        extern_inlines.push_back(func);
      }
      count++;
    }
  );
  os << "\n";
  os << "/* --------------------- sifive_nb2qspi0 ------------ */\n";
  while (!extern_inlines.empty()) {
    func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_dec(func, "sifive_nb2qspi0");
    delete func;
  }
  os << "\n";
}

void sifive_nb2qspi0::define_inlines()
{
  Inline* control_base_func;
  Inline* control_size_func;
  Inline* clock_func;
  Inline* pinmux_func;
  Inline* pinmux_output_selector_func;
  Inline* pinmux_source_selector_func;

  int count = 0;

  dtb.match(
    std::regex(compat_string),
    [&](node n) {

      /* Clock driving the QSPI peripheral */
      std::string clock_value = "NULL";
      n.maybe_tuple(
        "clocks", tuple_t<node>(),
        [&](){},
        [&](node m) {
          clock_value = "(struct metal_clock *)&__metal_dt_" + m.handle() + ".clock";
        });

      /* Pinmux */
      std::string pinmux_value = "NULL";
      uint32_t pinmux_dest = 0;
      uint32_t pinmux_source = 0;
      n.maybe_tuple(
        "pinmux", tuple_t<node, uint32_t, uint32_t>(),
        [&](){},
        [&](node m, uint32_t dest, uint32_t source) {
          pinmux_value = "(struct __metal_driver_sifive_nb2gpio0 *)&__metal_dt_" + m.handle();
          pinmux_dest = dest;
          pinmux_source = source;
        });

      /* Define inline functions */
      if (count == 0) {
        control_base_func = create_inline_def("control_base",
                                              "unsigned long",
                                              "(uintptr_t)qspi == (uintptr_t)&__metal_dt_" + n.handle(),
                                              platform_define(n, METAL_BASE_ADDRESS_LABEL),
                                              "struct metal_qspi *qspi");

        control_size_func = create_inline_def("control_size",
                                              "unsigned long",
                                              "(uintptr_t)qspi == (uintptr_t)&__metal_dt_" + n.handle(),
                                              platform_define(n, METAL_SIZE_LABEL),
                                              "struct metal_qspi *qspi");

        clock_func = create_inline_def("clock",
                                       "struct metal_clock *",
                                       "(uintptr_t)qspi == (uintptr_t)&__metal_dt_" + n.handle(),
                                       clock_value,
                                       "struct metal_qspi *qspi");

        pinmux_func = create_inline_def("pinmux",
                                        "struct __metal_driver_sifive_nb2gpio0 *",
                                        "(uintptr_t)qspi == (uintptr_t)&__metal_dt_" + n.handle(),
                                        pinmux_value,
                                        "struct metal_qspi *qspi");

        pinmux_output_selector_func = create_inline_def("pinmux_output_selector",
                                                        "unsigned long",
                                                        "(uintptr_t)qspi == (uintptr_t)&__metal_dt_" + n.handle(),
                                                        std::to_string(pinmux_dest),
                                                        "struct metal_qspi *qspi");

        pinmux_source_selector_func = create_inline_def("pinmux_source_selector",
                                                        "unsigned long",
                                                        "(uintptr_t)qspi == (uintptr_t)&__metal_dt_" + n.handle(),
                                                        std::to_string(pinmux_source),
                                                        "struct metal_qspi *qspi");
      } else { /* count > 0 */
        add_inline_body(control_base_func,
                        "(uintptr_t)qspi == (uintptr_t)&__metal_dt_" + n.handle(),
                        platform_define(n, METAL_BASE_ADDRESS_LABEL));

        add_inline_body(control_size_func,
                        "(uintptr_t)qspi == (uintptr_t)&__metal_dt_" + n.handle(),
                        platform_define(n, METAL_SIZE_LABEL));

        add_inline_body(clock_func,
                        "(uintptr_t)qspi == (uintptr_t)&__metal_dt_" + n.handle(),
                        clock_value);

        add_inline_body(pinmux_func,
                        "(uintptr_t)qspi == (uintptr_t)&__metal_dt_" + n.handle(),
                        pinmux_value);

        add_inline_body(pinmux_output_selector_func,
                        "(uintptr_t)qspi == (uintptr_t)&__metal_dt_" + n.handle(),
                        std::to_string(pinmux_dest));

        add_inline_body(pinmux_source_selector_func,
                        "(uintptr_t)qspi == (uintptr_t)&__metal_dt_" + n.handle(),
                        std::to_string(pinmux_source));
      }

      count++;
    });

  if (num_qspis != 0) {
    add_inline_body(control_base_func, "else", "0");
    add_inline_body(control_size_func, "else", "0");
    add_inline_body(clock_func, "else", "0");
    add_inline_body(pinmux_func, "else", "0");
    add_inline_body(pinmux_output_selector_func, "else", "0");
    add_inline_body(pinmux_source_selector_func, "else", "0");

    emit_inline_def(control_base_func, "sifive_nb2qspi0");
    delete control_base_func;
    emit_inline_def(control_size_func, "sifive_nb2qspi0");
    delete control_size_func;
    emit_inline_def(clock_func, "sifive_nb2qspi0");
    delete clock_func;
    emit_inline_def(pinmux_func, "sifive_nb2qspi0");
    delete pinmux_func;
    emit_inline_def(pinmux_output_selector_func, "sifive_nb2qspi0");
    delete pinmux_output_selector_func;
    emit_inline_def(pinmux_source_selector_func, "sifive_nb2qspi0");
    delete pinmux_source_selector_func;
  }
  os << "\n";
}

void sifive_nb2qspi0::declare_structs()
{
  dtb.match(
    std::regex(compat_string),
    [&](node n) {
      emit_struct_decl("sifive_nb2qspi0", n);
    }
  );
}

void sifive_nb2qspi0::define_structs()
{
  dtb.match(
    std::regex(compat_string),
    [&](node n) {
      emit_struct_begin("sifive_nb2qspi0", n);

      emit_struct_field("qspi.vtable", "&__metal_driver_vtable_sifive_nb2qspi0.qspi");

      emit_struct_end();
    });
}

void sifive_nb2qspi0::create_handles()
{
  emit_def("__METAL_DT_MAX_QSPIS", std::to_string(num_qspis));

  emit_struct_pointer_begin("sifive_nb2qspi0", "__metal_qspi_table", "[]");
  if (num_qspis) {
    int i = 0;
    dtb.match(
      std::regex(compat_string),
      [&](node n) {
        os << "\t\t\t\t\t&__metal_dt_" << n.handle();

        if ((i + 1) == num_qspis) {
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
