/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <sifive_nb2otp.h>

#include <regex>

sifive_nb2otp::sifive_nb2otp(std::ostream &os, const fdt &dtb)
  : Device(os, dtb, "sifive,nb2otp")
{
  /* Count the number of OTP */
  num_otp = 0;
  dtb.match(
    std::regex(compat_string),
    [&](node n) {
      num_otp += 1;
    });
}

void sifive_nb2otp::create_defines()
{
  dtb.match(
    std::regex(compat_string),
    [&](node n) {
      uint32_t num_interrupts = n.get_fields_count<uint32_t>("interrupts");

      emit_def("__METAL_" + n.handle_cap() + "_INTERRUPTS", std::to_string(num_interrupts));

      if(num_interrupts > max_interrupts) {
	max_interrupts = num_interrupts;
      }
	emit_def("METAL_MAX_OTP_INTERRUPTS", std::to_string(max_interrupts));
	});
}

void sifive_nb2otp::include_headers()
{
  dtb.match(
    std::regex(compat_string),
    [&](node n) {
      emit_include(compat_string);
    });
}

void sifive_nb2otp::declare_inlines()
{
  Inline* func;
  std::list<Inline *> extern_inlines;
  int count = 0;
  
  dtb.match(
    std::regex(compat_string),
    [&](node n) {
      if (count == 0) {
	func = create_inline_dec("base",
				 "unsigned long long",
				 "struct metal_otp *otp");
	extern_inlines.push_back(func);

	func = create_inline_dec("size",
				 "unsigned long long",
				 "struct metal_otp *otp");
	extern_inlines.push_back(func);

	func = create_inline_dec("num_interrupts",
				 "int",
				 "struct metal_otp *otp");
	extern_inlines.push_back(func);

	func = create_inline_dec("interrupt_parent",
				 "struct metal_interrupt *",
				 "struct metal_otp *otp");
	extern_inlines.push_back(func);

	func = create_inline_dec("interrupt_lines",
				 "int",
				 "struct metal_otp *otp", "int idx");
	extern_inlines.push_back(func);
      }
      count++;
    }
  );
  os << "\n";
  os << "/* --------------------- sifive_nb2otp ------------ */\n";
  while (!extern_inlines.empty()) {
    func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_dec(func, "sifive_nb2otp");
    delete func;
  }
  os << "\n";
}

void sifive_nb2otp::define_inlines()
{
  Inline* func;
  Inline *func1, *func2, *func3;
  Inline *func4, *func5;
  std::list<Inline *> extern_inlines;

  int count = 0;
  dtb.match(
    std::regex(compat_string),
    [&](node n) {
      if (num_otp == 0) {
	func = create_inline_def("base",
				 "unsigned long long",
				 "empty",
				 "0",
				 "struct metal_otp *otp");
	extern_inlines.push_back(func);

	func = create_inline_def("size",
				 "unsigned long long",
				 "empty",
				 "0",
				 "struct metal_otp *otp");
	extern_inlines.push_back(func);

	func = create_inline_def("num_interrupts",
				 "int",
				 "empty",
				 "0",
				 "struct metal_otp *otp");
	extern_inlines.push_back(func);
      } else {
	if (count == 0) {
	  func1 = create_inline_def("base",
				   "unsigned long long",
				   "(uintptr_t)otp == (uintptr_t)&__metal_dt_" + n.handle(),
				   platform_define(n, METAL_BASE_ADDRESS_LABEL),
				   "struct metal_otp *otp");
	  extern_inlines.push_back(func1);

	  func2 = create_inline_def("size",
				   "unsigned long long",
				   "(uintptr_t)otp == (uintptr_t)&__metal_dt_" + n.handle(),
				   platform_define(n, METAL_SIZE_LABEL),
				   "struct metal_otp *otp");
	  extern_inlines.push_back(func2);

	  func3 = create_inline_def("num_interrupts",
				   "int",
				   "(uintptr_t)otp == (uintptr_t)&__metal_dt_" + n.handle(),
				   "METAL_MAX_OTP_INTERRUPTS",
				   "struct metal_otp *otp");
	  extern_inlines.push_back(func3);
	}
	if (count > 0) {
	  add_inline_body(func1,
			  "(uintptr_t)otp == (uintptr_t)&__metal_dt_" + n.handle(),
			  platform_define(n, METAL_BASE_ADDRESS_LABEL));
	  add_inline_body(func2,
			  "(uintptr_t)otp == (uintptr_t)&__metal_dt_" + n.handle(),
			  platform_define(n, METAL_SIZE_LABEL));
	  add_inline_body(func3,
			  "(uintptr_t)otp == (uintptr_t)&__metal_dt_" + n.handle(),
			  "METAL_MAX_OTP_INTERRUPTS");
	}
	if ((count + 1) == num_otp) {
	  add_inline_body(func1, "else", "0");
	  add_inline_body(func2, "else", "0");
	  add_inline_body(func3, "else", "0");
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
				     "struct metal_otp *otp");
	    extern_inlines.push_back(func);
	  }
	},
	[&](node m) {
	  std::string value = "(struct metal_interrupt *)&__metal_dt_"
			      + m.handle() + ".controller";
	  if (count == 0) {
	    func4 = create_inline_def("interrupt_parent",
				     "struct metal_interrupt *",
				     "(uintptr_t)otp == (uintptr_t)&__metal_dt_" + n.handle(),
				     value,
				     "struct metal_otp *otp");
	    extern_inlines.push_back(func4);
	  }
	  if (count > 0) {
	    add_inline_body(func4,
			    "(uintptr_t)otp == (uintptr_t)&__metal_dt_" + n.handle(),
			    value);
	  }
	  if ((count + 1) == num_otp) {
	    add_inline_body(func4, "else", "0");
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
				     "struct metal_otp *otp", "int idx");
	    extern_inlines.push_back(func);
	  }
	},
	[&](int i, uint32_t irline){
	  if ((count == 0) && (i == 0)) {
	    func5 = create_inline_def("interrupt_lines",
				      "int",
				      "((uintptr_t)otp == (uintptr_t)&__metal_dt_" 
				       + n.handle() + ") && (" + "idx == " + std::to_string(i) + ")",
				      std::to_string(irline),
				      "struct metal_otp *otp", "int idx");
	    extern_inlines.push_back(func5);
	  } else if (((count + 1) == num_otp) && ((i + 1) == max_interrupts)) {
	    add_inline_body(func5,
			    "(((uintptr_t)otp == (uintptr_t)&__metal_dt_" + n.handle()
			     + ") && (" + "idx == " + std::to_string(i) + "))",
			    std::to_string(irline));
	    add_inline_body(func5, "else", "0");
	  } else {
	    add_inline_body(func5,
			    "(((uintptr_t)otp == (uintptr_t)&__metal_dt_" + n.handle()
			     + ") && (" + "idx == " + std::to_string(i) + "))",
			    std::to_string(irline));
	  }
	});

      count++;
    }
  );
  os << "\n";
  os << "/* --------------------- sifive_nb2otp ------------ */\n";
  while (!extern_inlines.empty()) {
    func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_def(func, "sifive_nb2otp");
    delete func;
  }
  os << "\n";
}

void sifive_nb2otp::declare_structs()
{
  dtb.match(
    std::regex(compat_string),
    [&](node n) {
      emit_struct_decl("sifive_nb2otp", n);
    }
  );
}

void sifive_nb2otp::define_structs()
{
  dtb.match(
    std::regex(compat_string),
    [&](node n) {
      emit_struct_begin("sifive_nb2otp", n);

      emit_struct_field("otp.vtable", "&__metal_driver_vtable_sifive_nb2otp.otp");

      emit_struct_end();
    });
}

void sifive_nb2otp::create_handles()
{
  emit_def("__METAL_DT_MAX_OTP", std::to_string(num_otp));

  emit_struct_pointer_begin("sifive_nb2otp", "__metal_otp_table", "[]");
  if (num_otp) {
    int i = 0;
    dtb.match(
      std::regex(compat_string),
      [&](node n) {
	os << "\t\t\t\t\t&__metal_dt_" << n.handle();

	if ((i + 1) == num_otp) {
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

