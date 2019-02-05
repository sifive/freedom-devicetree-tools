/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_STDOUT_PATH__H
#define __METAL_HEADER_STDOUT_PATH__H

#include "metal_header/device.h"

#include <regex>

class stdout_path : public Device {
  public:
    stdout_path(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "")
    {}

    void create_machine_macros() {}

    void create_defines() {}

    void include_headers() {}

    void declare_structs() {}

    void define_structs() {}

    void create_handles()
    {
      dtb.chosen(
	"stdout-path",
        tuple_t<std::string>(),
        [&](std::string info) {
	  if(info.find(":") == decltype(info)::npos) {
	    return;
          }

	  auto path = info.substr(0, info.find(":"));
	  auto target = dtb.node_by_path(path);
	  emit_def_handle("__METAL_DT_STDOUT_UART_HANDLE", target, ".uart");

	  auto baud = info.substr(info.find(":")+1);
	  emit_def("__METAL_DT_STDOUT_UART_BAUD", baud);
	});
    }
};

#endif
