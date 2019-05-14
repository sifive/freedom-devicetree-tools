/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_GPIO0__H
#define __METAL_HEADER_SIFIVE_GPIO0__H

#include "metal_header/device.h"

#include <regex>

class sifive_gpio0 : public Device {
  public:
    int num_gpios = 0;
    uint32_t max_interrupts = 0;

    sifive_gpio0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,gpio0")
    {
      /* Count the number of GPIOs */
      num_gpios = 0;
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  num_gpios += 1;
	});
    }

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
 
      emit_def("METAL_MAX_GPIO_INTERRUPTS", std::to_string(max_interrupts));
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
	    func = create_inline_dec("base",
				     "unsigned long",
				     "struct metal_gpio *gpio");
	    extern_inlines.push_back(func);

	    func = create_inline_dec("size",
				     "unsigned long",
				     "struct metal_gpio *gpio");
	    extern_inlines.push_back(func);

	    func = create_inline_dec("num_interrupts",
				     "int",
				     "struct metal_gpio *gpio");
	    extern_inlines.push_back(func);

	    func = create_inline_dec("interrupt_parent",
				     "struct metal_interrupt *",
				     "struct metal_gpio *gpio");
	    extern_inlines.push_back(func);

	    func = create_inline_dec("interrupt_lines",
				     "int",
				     "struct metal_gpio *gpio", "int idx");
	    extern_inlines.push_back(func);
	  }
          count++;
	}
      );
      os << "\n";
      os << "/* --------------------- sifive_gpio0 ------------ */\n";
      while (!extern_inlines.empty()) {
	func = extern_inlines.front();
	extern_inlines.pop_front();
	emit_inline_dec(func, "sifive_gpio0");
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
	    func = create_inline_def("base",
				     "unsigned long",
				     "empty",
				     "0",
				     "struct metal_gpio *gpio");
	    extern_inlines.push_back(func);

	    func = create_inline_def("size",
				     "unsigned long",
				     "empty",
				     "0",
				     "struct metal_gpio *gpio");
	    extern_inlines.push_back(func);

	    func = create_inline_def("num_interrupts",
				     "int",
				     "empty",
				     "0",
				     "struct metal_gpio *gpio");
	    extern_inlines.push_back(func);
	  } else {
	    if (count == 0) {
	      func = create_inline_def("base",
				       "unsigned long",
				       "(uintptr_t)gpio == (uintptr_t)&__metal_dt_" + n.handle(),
                                       platform_define(n, METAL_BASE_ADDRESS_LABEL),
				       "struct metal_gpio *gpio");
	      add_inline_body(func, "else", "0");
	      extern_inlines.push_back(func);

	      func = create_inline_def("size",
				       "unsigned long",
				       "(uintptr_t)gpio == (uintptr_t)&__metal_dt_" + n.handle(),
                                       platform_define(n, METAL_SIZE_LABEL),
				       "struct metal_gpio *gpio");
	      add_inline_body(func, "else", "0");
	      extern_inlines.push_back(func);

	      func = create_inline_def("num_interrupts",
				       "int",
				       "(uintptr_t)gpio == (uintptr_t)&__metal_dt_" + n.handle(),
				       "METAL_MAX_GPIO_INTERRUPTS",
				       "struct metal_gpio *gpio");
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
					 "struct metal_gpio *gpio");
		extern_inlines.push_back(func);
	      }
	    },
	    [&](node m) {
	      if (count == 0) {
		std::string value = "(struct metal_interrupt *)&__metal_dt_"
		                  + m.handle() + ".controller";
		func = create_inline_def("interrupt_parent",
					 "struct metal_interrupt *",
					 "(uintptr_t)gpio == (uintptr_t)&__metal_dt_" + n.handle(),
					 value,
					 "struct metal_gpio *gpio");
		add_inline_body(func, "else", "NULL");
		extern_inlines.push_back(func);
	      }
	    });

	  n.maybe_tuple_index(
	    "interrupts", tuple_t<uint32_t>(),
	    [&](){
	      if (count == 0) {
		func = create_inline_def("interrupt_lines",
					 "int",
					 "empty",
					 "0",
					 "struct metal_gpio *gpio", "int idx");
		extern_inlines.push_back(func);
	      }
	    },
	    [&](int i, uint32_t irline){
	      if (i == 0) {
		func = create_inline_def("interrupt_lines",
					 "int",
					 "idx == " + std::to_string(i),
					 std::to_string(irline),
					 "struct metal_gpio *gpio", "int idx");
		extern_inlines.push_back(func);
	      } else if ((i + 1) == max_interrupts) {
		add_inline_body(func, "idx == " + std::to_string(i), std::to_string(irline));
		add_inline_body(func, "else", "0");
	      } else {
		add_inline_body(func, "idx == " + std::to_string(i), std::to_string(irline));
	      }
	    });

	  count++;
	}
      );
      os << "\n";
      os << "/* --------------------- sifive_gpio0 ------------ */\n";
      while (!extern_inlines.empty()) {
	func = extern_inlines.front();
	extern_inlines.pop_front();
	emit_inline_def(func, "sifive_gpio0");
	delete func;
      }
      os << "\n";
    }

    void declare_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_decl("sifive_gpio0", n);
	}
      );
    }

    void define_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_begin("sifive_gpio0", n);

	  emit_struct_field("gpio.vtable", "&__metal_driver_vtable_sifive_gpio0.gpio");

	  emit_struct_end();
	});
    }

    void create_handles()
    {
      emit_def("__MEE_DT_MAX_GPIOS", std::to_string(num_gpios));

      emit_struct_pointer_begin("sifive_gpio0", "__metal_gpio_table", "[]");
      if (num_gpios) {
	int i = 0;
	dtb.match(
	  std::regex(compat_string),
	  [&](node n) {
	    os << "\t\t\t\t\t&__metal_dt_" << n.handle();

	    if ((i + 1) == num_gpios) {
	      os << "};\n\n";
	    } else {
	      os << ",\n";
	    }

	    i += 1;
	  });
      } else {
	emit_struct_pointer_end("NULL");
}
    }
};

#endif
