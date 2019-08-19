/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stdout_path.h>

#include <regex>

stdout_path::stdout_path(std::ostream &os, const fdt &dtb)
    : Device(os, dtb, "") {}

void stdout_path::create_handles() {
  dtb.chosen("stdout-path", tuple_t<std::string>(), [&](std::string info) {
    if (info.find(":") != decltype(info)::npos) {
      auto path = info.substr(0, info.find(":"));
      auto target = dtb.node_by_path(path);
      auto baud = info.substr(info.find(":") + 1);

      emit_def_handle("__METAL_DT_STDOUT_UART_HANDLE", target, ".uart");
      emit_def("__METAL_DT_STDOUT_UART_BAUD", baud);
    } else {
      auto target = dtb.node_by_path(info);

      emit_def_handle("__METAL_DT_STDOUT_UART_HANDLE", target, ".uart");
      /* If no baud rate is specified, default to 115200 */
      emit_def("__METAL_DT_STDOUT_UART_BAUD", "115200");
    }
  });
}
