/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#include "constants_group.h"

#include <regex>

ConstantsGroup::ConstantsGroup(const fdt &dtb)
  : SectionGroup(Memory(), Phdr(), Memory(), Phdr())
{
  add_constant("__stack_size = DEFINED(__stack_size) ? __stack_size : 0x400;");
  add_constant("PROVIDE(__stack_size = __stack_size);");
  add_constant("__heap_size = DEFINED(__heap_size) ? __heap_size : 0x800;");

  string boot_hart = "0";
  int chicken_bit = 0;

  dtb.chosen(
    "metal,boothart",
    tuple_t<node>(),
    [&](const node n) {
      boot_hart = n.instance();
    });

  dtb.match(
    std::regex("cpu"),
    [&](const node n) {
      auto compatibles = n.get_fields<string>("compatible");

      for (auto it = compatibles.begin(); it != compatibles.end(); it++) {
        if (it->find("bullet") != string::npos) {
          chicken_bit = 1;
          break;
        }
      }
    });

  add_constant("PROVIDE(__metal_boot_hart = " + boot_hart + ");");
  add_constant("PROVIDE(__metal_chicken_bit = " + std::to_string(chicken_bit) + ");");
}

void ConstantsGroup::add_constant(string constant) {
  trailing_commands.push_back(constant);
}

string ConstantsGroup::describe() {
  string description = "";

  for (auto it = trailing_commands.begin(); it != trailing_commands.end(); it++) {
    description += "\t" + *it + "\n";
  }

  return description;
}

