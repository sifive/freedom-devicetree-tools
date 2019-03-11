/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_TEMPLATE_DEVICE__H
#define __METAL_HEADER_TEMPLATE_DEVICE__H

#include "metal_header/device.h"

#include <regex>

class sifive_fu540_c000_l2 : public Device {
  public:
    sifive_fu540_c000_l2(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,ccache0")
    {}

    void include_headers()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_include("sifive,fu540-c000,l2");
	});
    }

    void declare_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_decl("sifive_fu540_c000_l2", n);
	}
      );
    }

    void define_structs()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_struct_begin("sifive_fu540_c000_l2", n);

	  emit_struct_field("vtable", "&__metal_driver_vtable_sifive_fu540_c000_l2");
	  emit_struct_field("cache.vtable", "&__metal_driver_vtable_sifive_fu540_c000_l2.cache");

	  n.named_tuples(
	    "reg-names", "reg",
	    "config", tuple_t<node>(), [&](node base) {
	      emit_struct_field_node("config_base", base, "");
	    });

	  emit_struct_end();
	});
    }

    void create_handles()
    {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_def_handle("__METAL_DT_SIFIVE_FU540_C000_L2_HANDLE", n, "");
	});
    }
};

#endif
