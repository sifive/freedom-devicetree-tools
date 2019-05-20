/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_SPI0__H
#define __METAL_HEADER_SIFIVE_SPI0__H

#include "metal_header/device.h"

#include <regex>

class sifive_spi0 : public Device {
  public:
    int num_spis;

    sifive_spi0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,spi0")
    {
      /* Count the number of SPIs */
      num_spis = 0;
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  num_spis += 1;
	});
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
				     "struct metal_spi *spi");
	    extern_inlines.push_back(func);

	    func = create_inline_dec("control_size",
				     "unsigned long",
				     "struct metal_spi *spi");
	    extern_inlines.push_back(func);

	    func = create_inline_dec("pinmux",
				     "struct __metal_driver_sifive_gpio0 *",
				     "struct metal_spi *spi");
	    extern_inlines.push_back(func);

	    func = create_inline_dec("pinmux_output_selector",
				     "unsigned long",
				     "struct metal_spi *spi");
	    extern_inlines.push_back(func);

	    func = create_inline_dec("pinmux_source_selector",
				     "unsigned long",
				     "struct metal_spi *spi");
	    extern_inlines.push_back(func);
	  }
          count++;
	}
      );
      os << "\n";
      os << "/* --------------------- sifive_spi0 ------------ */\n";
      while (!extern_inlines.empty()) {
	func = extern_inlines.front();
	extern_inlines.pop_front();
	emit_inline_dec(func, "sifive_spi0");
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
	  if (count == 0) {
	    func = create_inline_def("control_base",
				     "unsigned long",
				     "(uintptr_t)spi == (uintptr_t)&__metal_dt_" + n.handle(),
                                     platform_define(n, METAL_BASE_ADDRESS_LABEL),
				     "struct metal_spi *spi");
	    add_inline_body(func, "else", "0");
	    extern_inlines.push_back(func);

	    func = create_inline_def("control_size",
				     "unsigned long",
				     "(uintptr_t)spi == (uintptr_t)&__metal_dt_" + n.handle(),
                                     platform_define(n, METAL_SIZE_LABEL),
				     "struct metal_spi *spi");
	    add_inline_body(func, "else", "0");
	    extern_inlines.push_back(func);
	  }

	  n.maybe_tuple(
	    "clocks", tuple_t<node>(),
	    [&](){
	      if (count == 0) {
		func = create_inline_def("clock",
					 "struct metal_clock *",
					 "empty",
					 "NULL",
					 "struct metal_spi *spi");
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
					 "struct metal_spi *spi");
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
					 "struct metal_spi *spi");
		extern_inlines.push_back(func);

		func = create_inline_def("pinmux_output_selector",
					 "unsigned long",
					 "empty",
					 "0",
					 "struct metal_spi *spi");
		extern_inlines.push_back(func);

		func = create_inline_def("pinmux_source_selector",
					 "unsigned long",
					 "empty",
					 "0",
					 "struct metal_spi *spi");
		extern_inlines.push_back(func);
	      }
	    },
	    [&](node m, uint32_t dest, uint32_t source) {
	      if (count == 0) {
		func = create_inline_def("pinmux",
					 "struct __metal_driver_sifive_gpio0 *",
					 "empty",
					 "(struct __metal_driver_sifive_gpio0 *)&__metal_dt_" + m.handle(),
					 "struct metal_spi *spi");
		extern_inlines.push_back(func);

		func = create_inline_def("pinmux_output_selector",
					 "unsigned long",
					 "empty",
					 std::to_string(dest),
					 "struct metal_spi *spi");
		extern_inlines.push_back(func);

		func = create_inline_def("pinmux_source_selector",
					 "unsigned long",
					 "empty",
					 std::to_string(source),
					 "struct metal_spi *spi");
		extern_inlines.push_back(func);
	      }
	    });

	  count++;
	}
      );
      os << "\n";
      os << "/* --------------------- sifive_spi0 ------------ */\n";
      while (!extern_inlines.empty()) {
	func = extern_inlines.front();
	extern_inlines.pop_front();
	emit_inline_def(func, "sifive_spi0");
	delete func;
      }
      os << "\n";
    }

    void declare_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_decl("sifive_spi0", n);
	}
      );
    }

    void define_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_begin("sifive_spi0", n);

	  emit_struct_field("spi.vtable", "&__metal_driver_vtable_sifive_spi0.spi");

	  emit_struct_end();
	});
    }

    void create_handles()
    {
      emit_def("__METAL_DT_MAX_SPIS", std::to_string(num_spis));

      emit_struct_pointer_begin("sifive_spi0", "__metal_spi_table", "[]");
      if (num_spis) {
	int i = 0;
	dtb.match(
	  std::regex(compat_string),
	  [&](node n) {
	    os << "\t\t\t\t\t&__metal_dt_" << n.handle();

	    if ((i + 1) == num_spis) {
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
};

#endif
