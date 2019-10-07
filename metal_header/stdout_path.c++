/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stdout_path.h>

#include <iostream>
#include <regex>
#include <string>

using std::string;

stdout_path::stdout_path(std::ostream &os, const fdt &dtb)
    : Device(os, dtb, "") {}

void stdout_path::create_handles() {
  dtb.chosen("stdout-path", tuple_t<std::string>(), [&](std::string info) {
    /* If no baud rate is specified, default to 115200 */
    string baud = "115200";

    auto get_target_node = [&]() -> const node {
      if (info.find(":") != decltype(info)::npos) {
        auto path = info.substr(0, info.find(":"));
        auto target = dtb.node_by_path(path);
        baud = info.substr(info.find(":") + 1);

        return target;
      } else {
        auto target = dtb.node_by_path(info);

        return target;
      }
    };

    auto target = get_target_node();

    /* Device-specific stdout-path behavior */
    auto compat_strings = target.get_fields<string>("compatible");
    for (auto it = compat_strings.begin(); it != compat_strings.end(); it++) {
      if (it->find("ucb,htif0") != string::npos) {
        emit_def_handle("__METAL_DT_STDOUT_UART_HANDLE", "uart", target,
                        ".uart");
        emit_def("__METAL_DT_STDOUT_UART_BAUD", baud);
        return;
      }
    }

    emit_def_handle("__METAL_DT_STDOUT_UART_HANDLE", target, ".uart");
    emit_def("__METAL_DT_STDOUT_UART_BAUD", baud);
  });
}
