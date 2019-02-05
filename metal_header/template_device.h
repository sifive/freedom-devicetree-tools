/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_TEMPLATE_DEVICE__H
#define __METAL_HEADER_TEMPLATE_DEVICE__H

#include "metal_header/device.h"

#include <regex>

class template_device : public Device {
  public:
    template_device(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "compat,example")
    {}

    void create_machine_macros() {}

    void create_defines()
    {
      /* Certain devices need to create #defines at the top of the header.
       * Those defines should be output here */
    }

    void include_headers()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_include(compat_string);
	});
    }

    void declare_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_decl("struct_example", n);
	}
      );
    }

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

    void create_handles()
    {
      /* Emit device handles here */
    }
};

#endif
