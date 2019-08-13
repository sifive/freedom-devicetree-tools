/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

/****************************************

  DEVICE TEMPLATE

  This file is a template for adding support for new hardware to Freedom Metal.

  To add a new device, create a copy of this file and rename all instances of
  "template_device" to the name of the device you're adding. Make sure you
  remember to change the #ifndef at the top too!

****************************************/

#include <template_device.h>

#include <regex>

template_device::template_device(std::ostream &os, const fdt &dtb)
    : Device(os, dtb, "compat,example") {}

/* "Machine macros" are #defines used by certain drivers to provide more
 * information about device capabilities during the driver build. */
void template_device::create_machine_macros() {}

/* Certain devices need to create #defines at the top of the header.
 * Those defines should be output here */
void template_device::create_defines() {}

/* This method includes the header file */
void template_device::include_headers() {
  /* If all you need is metal/driver/compat_example.h, you can leave this
   * alone */
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_include(compat_string); });
}

/* This method declares inline functions in metal-inline.h */
void template_device::declare_inlines() {}

/* This method defines inline functions in metal.h */
void template_device::define_inlines() {}

/* This method declares the device driver struct */
void template_device::declare_structs() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_struct_decl("struct_example", n); });
}

/* This method defines the device driver struct, and is also where you
 * should output the values for the struct fields */
void template_device::define_structs() {
  dtb.match(std::regex(compat_string), [&](node n) {
    emit_struct_begin("struct_example", n);

    /* Emit struct fields here */

    emit_struct_end();
  });
}

/* This method creates device handles */
void template_device::create_handles() {}
