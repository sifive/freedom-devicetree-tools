/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

/****************************************

  DEVICE TEMPLATE

  This file is a template for adding support for new hardware to Freedom Metal.

  To add a new device, create a copy of this file and rename all instances of
  "template_device" to the name of the device you're adding. Make sure you
  remember to change the #ifndef at the top too!

****************************************/

#ifndef __METAL_HEADER_TEMPLATE_DEVICE__H
#define __METAL_HEADER_TEMPLATE_DEVICE__H

#include "metal_header/device.h"

#include <regex>

class template_device : public Device {
  public:
    template_device(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "compat,example")
    {}

    /* Certain devices need to create #defines at the top of the header.
     * Those defines should be output here */
    void create_defines() {}

    /* This method includes the header file */
    void include_headers()
    {
      /* If all you need is metal/driver/compat_example.h, you can leave this
       * alone */
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_include(compat_string);
	});
    }

    /* This method declares the device driver struct */
    void declare_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_decl("struct_example", n);
	}
      );
    }

    /* This method defines the device driver struct, and is also where you
     * should output the values for the struct fields */
    void define_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_begin("struct_example", n);

	  /* Emit struct fields here */

	  emit_struct_end();
	});
    }

    /* This method creates device handles */
    void create_handles() {}
};

#endif
