/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */


#include <synopsys_i2s_slave_v1_11a.h>

#include <regex>

synopsys_i2s_slave_v1_11a::synopsys_i2s_slave_v1_11a(std::ostream &os, const fdt &dtb)
    : Device(os, dtb, "synopsys_i2s_slave_v1_11a") {}


/* Certain devices need to create #defines at the top of the header.
 * Those defines should be output here */
void synopsys_i2s_slave_v1_11a::create_defines() {

  dtb.match(std::regex(compat_string), [&](node n) {
    num_i2s += 1;

    uint32_t num_interrupts = n.get_fields_count<uint32_t>("interrupts");

    emit_def("__METAL_" + n.handle_cap() + "_INTERRUPTS",
             std::to_string(num_interrupts));
     if (num_interrupts > max_interrupts) {
      max_interrupts = num_interrupts;
    }
  });
  emit_def("METAL_MAX_I2S_SLAVE_INTERRUPTS", std::to_string(max_interrupts));
}

/* This method includes the header file */
void synopsys_i2s_slave_v1_11a::include_headers() {
  /* If all you need is metal/driver/compat_example.h, you can leave this
   * alone */
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_include("synopsys_i2s"); });
}


/* This method declares the device driver struct */
void synopsys_i2s_slave_v1_11a::declare_structs() {

  dtb.match(std::regex(compat_string),
            [&](node n) { emit_struct_decl("synopsys_i2s", n); });
}

/* This method creates device handles */
void synopsys_i2s_slave_v1_11a::create_handles() {

      emit_def("__METAL_DT_MAX_I2S_SLAVE", std::to_string(num_i2s));

  emit_struct_pointer_begin("synopsys_i2s", "__metal_i2s_slave_table", "[]");
  if (num_i2s) {
    int i = 0;
    dtb.match(std::regex(compat_string), [&](node n) {
      os << "\t\t\t\t\t&__metal_dt_" << n.handle();

      if ((i + 1) == num_i2s) {
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

/* This method defines inline functions in metal.h */
void synopsys_i2s_slave_v1_11a::define_inlines() {

Inline *base_func;
  Inline *size_func;
  Inline *num_int_func;
  Inline *int_parent_func;
  Inline *int_line_func;
  Inline *clock_func;


  int count = 0;

  dtb.match(std::regex(compat_string), [&](node n) {
    /* Interrupts */
    std::string int_parent_value = "NULL";
    n.maybe_tuple("interrupt-parent", tuple_t<node>(), [&]() {},
                  [&](node m) {
                    
                      int_parent_value =
                          "(struct metal_interrupt *)&__metal_dt_" +
                          m.handle() + ".controller";
                    
                  });
    std::string int_line_value = "0";
    n.maybe_tuple(
        "interrupts", tuple_t<uint32_t>(), [&]() {},
        [&](uint32_t irline) { int_line_value = std::to_string(irline); });

    /* Clock driving the I2C peripheral */
    std::string clock_value = "NULL";
    n.maybe_tuple("clocks", tuple_t<node>(), [&]() {},
                  [&](node m) {
                    clock_value = "(struct metal_clock *)&__metal_dt_" +
                                  m.handle() + ".clock";
                  });

    if (count == 0) {
      base_func = create_inline_def(
          "slave_control_base", "unsigned long",
          "(uintptr_t)i2s == (uintptr_t)&__metal_dt_" + n.handle(),
          platform_define(n, METAL_BASE_ADDRESS_LABEL),
          "struct metal_i2s *i2s");

      size_func = create_inline_def(
          "slave_control_size", "unsigned long",
          "(uintptr_t)i2s == (uintptr_t)&__metal_dt_" + n.handle(),
          platform_define(n, METAL_SIZE_LABEL), "struct metal_i2s *i2s");

      num_int_func = create_inline_def(
          "slave_num_interrupts", "int",
          "(uintptr_t)i2s == (uintptr_t)&__metal_dt_" + n.handle(),
          "__METAL_" + n.handle_cap() + "_INTERRUPTS", "struct metal_i2s *i2s");

      int_parent_func = create_inline_def(
          "slave_interrupt_parent", "struct metal_interrupt *",
          "(uintptr_t)i2s == (uintptr_t)&__metal_dt_" + n.handle(),
          int_parent_value, "struct metal_i2s *i2s");

      int_line_func = create_inline_def(
          "slave_interrupt_line", "int",
          "(uintptr_t)i2s == (uintptr_t)&__metal_dt_" + n.handle(),
          int_line_value, "struct metal_i2s *i2s");

      clock_func = create_inline_def(
          "slave_clock", "struct metal_clock *",
          "(uintptr_t)i2s == (uintptr_t)&__metal_dt_" + n.handle(),
          clock_value, "struct metal_i2s *i2s");

    } else { /* count > 0 */
      add_inline_body(base_func,
                      "(uintptr_t)i2s == (uintptr_t)&__metal_dt_" + n.handle(),
                      platform_define(n, METAL_BASE_ADDRESS_LABEL));

      add_inline_body(size_func,
                      "(uintptr_t)i2s == (uintptr_t)&__metal_dt_" + n.handle(),
                      platform_define(n, METAL_SIZE_LABEL));

      add_inline_body(num_int_func,
                      "(uintptr_t)i2s == (uintptr_t)&__metal_dt_" + n.handle(),
                      "__METAL_" + n.handle_cap() + "_INTERRUPTS");

      add_inline_body(int_parent_func,
                      "(uintptr_t)i2s == (uintptr_t)&__metal_dt_" + n.handle(),
                      int_parent_value);

      add_inline_body(int_line_func,
                      "(uintptr_t)i2s == (uintptr_t)&__metal_dt_" + n.handle(),
                      int_line_value);

      add_inline_body(clock_func,
                      "(uintptr_t)i2s == (uintptr_t)&__metal_dt_" + n.handle(),
                      clock_value);


    };

    count += 1;
  });

  os << "\n";
  os << "/* --------------------- synopsys_i2s ------------ */\n";

  if (num_i2s != 0) {
    add_inline_body(base_func, "else", "0");
    add_inline_body(size_func, "else", "0");
    add_inline_body(num_int_func, "else", "0");
    add_inline_body(int_parent_func, "else", "0");
    add_inline_body(int_line_func, "else", "0");
    add_inline_body(clock_func, "else", "0");


    emit_inline_def(base_func, "synopsys_i2s");
    delete base_func;
    emit_inline_def(size_func, "synopsys_i2s");
    delete size_func;
    emit_inline_def(num_int_func, "synopsys_i2s");
    delete num_int_func;
    emit_inline_def(int_parent_func, "synopsys_i2s");
    delete int_parent_func;
    emit_inline_def(int_line_func, "synopsys_i2s");
    delete int_line_func;
    emit_inline_def(clock_func, "synopsys_i2s");
    delete clock_func;

  }
  os << "\n";

}
////////////////////////////////////////// metal-inline.h////////////////

/* This method declares inline functions in metal-inline.h */
void synopsys_i2s_slave_v1_11a::declare_inlines() {

Inline *func;
  std::list<Inline *> extern_inlines;
  int count = 0;

  dtb.match(std::regex(compat_string), [&](node n) {
    if (count == 0) {
      func = create_inline_dec("slave_control_base", "unsigned long",
                               "struct metal_i2s *i2s");
      extern_inlines.push_back(func);

      func = create_inline_dec("slave_control_size", "unsigned long",
                               "struct metal_i2s *i2s");
      extern_inlines.push_back(func);

      func =
          create_inline_dec("slave_num_interrupts", "int", "struct metal_i2s *i2s");
      extern_inlines.push_back(func);

      func = create_inline_dec("slave_interrupt_parent", "struct metal_interrupt *",
                               "struct metal_i2s *i2s");
      extern_inlines.push_back(func);

      func =
          create_inline_dec("slave_interrupt_line", "int", "struct metal_i2s *i2s");
      extern_inlines.push_back(func);

      func = create_inline_dec("slave_clock", "struct metal_clock *",
                               "struct metal_i2s *i2s");
      extern_inlines.push_back(func);

    }
    count++;
  });
  os << "\n";
  os << "/* --------------------- synopsys_i2s ------------ */\n";
  while (!extern_inlines.empty()) {
    func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_dec(func, "synopsys_i2s");
    delete func;
  }
  os << "\n";
 
}


/* This method defines the device driver struct, and is also where you
 * should output the values for the struct fields */
void synopsys_i2s_slave_v1_11a::define_structs() {
  
  dtb.match(std::regex(compat_string), [&](node n) {
    emit_struct_begin("synopsys_i2s", n);

    emit_struct_field("i2s.vtable",
                      "&__metal_driver_vtable_synopsys_i2s.i2s");

    emit_struct_end();
  });

}