/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_RISCV_PMP__H
#define __METAL_HEADER_RISCV_PMP__H

#include "metal_header/device.h"

#include <regex>

class riscv_pmp : public Device {
  public:
    riscv_pmp(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "riscv,pmp")
    {}

    void include_headers()
    {
      os << "#include <metal/pmp.h>\n";
    }

    void declare_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  os << "asm (\".weak __metal_dt_" << n.handle() << "\");\n";
	  os << "struct metal_pmp __metal_dt_" << n.handle() << ";\n\n";
	}
      );
    }

    void define_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_comment(n);
	  os << "struct metal_pmp __metal_dt_" << n.handle() << " = {\n";
	  emit_struct_field_platform_define("num_regions", n, METAL_NUM_REGIONS_LABEL);
	  emit_struct_end();
	});
    }

    void create_handles()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_comment(n);
	  os << "#define __METAL_DT_PMP_HANDLE" << " (&__metal_dt_" << n.handle() << ")\n\n";
	});
    }
};

#endif
