/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_UART0__H
#define __METAL_HEADER_SIFIVE_UART0__H

#include "metal_header/device.h"

#include <regex>

class sifive_uart0 : public Device {
  public:
    uint32_t max_interrupts = 0;

    sifive_uart0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,uart0")
    {}

    void create_defines()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  uint32_t num_interrupts = n.get_fields_count<uint32_t>("interrupts");

	  emit_def("__METAL_" + n.handle_cap() + "_INTERRUPTS", std::to_string(num_interrupts));

	  if(num_interrupts > max_interrupts) {
	    max_interrupts = num_interrupts;
	  }
	});
 
      emit_def("METAL_MAX_UART_INTERRUPTS", std::to_string(max_interrupts));
    }

    void include_headers()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_include(compat_string);
	});
    }

    void declare_inlines()
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
				     "struct metal_uart *uart");
	    extern_inlines.push_back(func);

	    func = create_inline_dec("control_size",
				     "unsigned long",
				     "struct metal_uart *uart");
	    extern_inlines.push_back(func);

	    func = create_inline_dec("num_interrupts",
				     "int",
				     "struct metal_uart *uart");
	    extern_inlines.push_back(func);

	    func = create_inline_dec("interrupt_parent",
				     "struct metal_interrupt *",
				     "struct metal_uart *uart");
	    extern_inlines.push_back(func);

	    func = create_inline_dec("interrupt_line",
				     "int",
				     "struct metal_uart *uart");
	    extern_inlines.push_back(func);

	    func = create_inline_dec("clock",
				     "struct metal_clock *",
				     "struct metal_uart *uart");
	    extern_inlines.push_back(func);

	    func = create_inline_dec("pinmux",
				     "struct __metal_driver_sifive_gpio0 *",
				     "struct metal_uart *uart");
	    extern_inlines.push_back(func);

	    func = create_inline_dec("pinmux_output_selector",
				     "unsigned long",
				     "struct metal_uart *uart");
	    extern_inlines.push_back(func);

	    func = create_inline_dec("pinmux_source_selector",
				     "unsigned long",
				     "struct metal_uart *uart");
	    extern_inlines.push_back(func);
	  }
          count++;
	}
      );
      os << "\n";
      os << "/* --------------------- sifive_uart0 ------------ */\n";
      while (!extern_inlines.empty()) {
	func = extern_inlines.front();
	extern_inlines.pop_front();
	emit_inline_dec(func, "sifive_uart0");
	delete func;
      }
      os << "\n";
    }

    void define_inlines()
    {
      Inline* func;
      std::list<Inline *> extern_inlines;

      int count = 0;
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  if (max_interrupts == 0) {
	    func = create_inline_def("control_base",
				     "unsigned long",
				     "empty",
				     "0",
				     "struct metal_uart *uart");
	    extern_inlines.push_back(func);

	    func = create_inline_def("control_size",
				     "unsigned long",
				     "empty",
				     "0",
				     "struct metal_uart *uart");
	    extern_inlines.push_back(func);

	    func = create_inline_def("num_interrupts",
				     "int",
				     "empty",
				     "0",
				     "struct metal_uart *uart");
	    extern_inlines.push_back(func);
	  } else {
	    if (count == 0) {
	      func = create_inline_def("control_base",
				       "unsigned long",
				       "(uintptr_t)uart == (uintptr_t)&__metal_dt_" + n.handle(),
                                       platform_define(n, METAL_BASE_ADDRESS_LABEL),
				       "struct metal_uart *uart");
	      add_inline_body(func, "else", "0");
	      extern_inlines.push_back(func);

	      func = create_inline_def("control_size",
				       "unsigned long",
				       "(uintptr_t)uart == (uintptr_t)&__metal_dt_" + n.handle(),
                                       platform_define(n, METAL_SIZE_LABEL),
				       "struct metal_uart *uart");
	      add_inline_body(func, "else", "0");
	      extern_inlines.push_back(func);

	      func = create_inline_def("num_interrupts",
				       "int",
				       "(uintptr_t)uart == (uintptr_t)&__metal_dt_" + n.handle(),
				       "METAL_MAX_UART_INTERRUPTS",
				       "struct metal_uart *uart");
	      add_inline_body(func, "else", "0");
	      extern_inlines.push_back(func);
	    }
	  }

	  n.maybe_tuple(
	    "interrupt-parent", tuple_t<node>(),
	    [&](){
	      if (count == 0) {
		func = create_inline_def("interrupt_parent",
					 "struct metal_interrupt *",
					 "empty",
					 "NULL",
					 "struct metal_uart *uart");
		extern_inlines.push_back(func);
	      }
	    },
	    [&](node m) {
	      if (count == 0) {
		std::string value = "(struct metal_interrupt *)&__metal_dt_"
		                  + m.handle() + ".controller";
		func = create_inline_def("interrupt_parent",
					 "struct metal_interrupt *",
					 "(uintptr_t)uart == (uintptr_t)&__metal_dt_" + n.handle(),
					 value,
					 "struct metal_uart *uart");
		add_inline_body(func, "else", "NULL");
		extern_inlines.push_back(func);
	      }
	    });

	  n.maybe_tuple(
	    "interrupts", tuple_t<uint32_t>(),
	    [&](){
	      if (count == 0) {
		func = create_inline_def("interrupt_line",
					 "int",
					 "empty",
					 "0",
					 "struct metal_uart *uart");
		extern_inlines.push_back(func);
	      }
	    },
	    [&](uint32_t irline){
	      if (count == 0) {
		func = create_inline_def("interrupt_line",
					 "int",
					 "empty",
					 std::to_string(irline),
					 "struct metal_uart *uart");
		extern_inlines.push_back(func);
	      }
	    });

	  n.maybe_tuple(
	    "clocks", tuple_t<node>(),
	    [&](){
	      if (count == 0) {
		func = create_inline_def("clock",
					 "struct metal_clock *",
					 "empty",
					 "NULL",
					 "struct metal_uart *uart");
		extern_inlines.push_back(func);
	      }
	    },
	    [&](node m) {
	      if (count == 0) {
		std::string value = "(struct metal_clock *)&__metal_dt_"
		                  + m.handle() + ".clock";
		func = create_inline_def("clock",
					 "struct metal_clock *",
					 "empty",
					 value,
					 "struct metal_uart *uart");
		extern_inlines.push_back(func);
	      }
	    });

	  n.maybe_tuple(
	    "pinmux", tuple_t<node, uint32_t, uint32_t>(),
	    [&](){
	      if (count == 0) {
		func = create_inline_def("pinmux",
					 "struct __metal_driver_sifive_gpio0 *",
					 "empty",
					 "NULL",
					 "struct metal_uart *uart");
		extern_inlines.push_back(func);

		func = create_inline_def("pinmux_output_selector",
					 "unsigned long",
					 "empty",
					 "0",
					 "struct metal_uart *uart");
		extern_inlines.push_back(func);

		func = create_inline_def("pinmux_source_selector",
					 "unsigned long",
					 "empty",
					 "0",
					 "struct metal_uart *uart");
		extern_inlines.push_back(func);
	      }
	    },
	    [&](node m, uint32_t dest, uint32_t source) {
	      if (count == 0) {
		func = create_inline_def("pinmux",
					 "struct __metal_driver_sifive_gpio0 *",
					 "empty",
					 "(struct __metal_driver_sifive_gpio0 *)&__metal_dt_" + m.handle(),
					 "struct metal_uart *uart");
		extern_inlines.push_back(func);

		func = create_inline_def("pinmux_output_selector",
					 "unsigned long",
					 "empty",
					 std::to_string(dest),
					 "struct metal_uart *uart");
		extern_inlines.push_back(func);

		func = create_inline_def("pinmux_source_selector",
					 "unsigned long",
					 "empty",
					 std::to_string(source),
					 "struct metal_uart *uart");
		extern_inlines.push_back(func);
	      }
	    });

	  count++;
	}
      );
      os << "\n";
      os << "/* --------------------- sifive_uart0 ------------ */\n";
      while (!extern_inlines.empty()) {
	func = extern_inlines.front();
	extern_inlines.pop_front();
	emit_inline_def(func, "sifive_uart0");
	delete func;
      }
      os << "\n";
    }

    void declare_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_decl("sifive_uart0", n);
	}
      );
    }

    void define_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_begin("sifive_uart0", n);

	  emit_struct_field("uart.vtable", "&__metal_driver_vtable_sifive_uart0.uart");

	  emit_struct_end();
	});
    }
};

#endif
