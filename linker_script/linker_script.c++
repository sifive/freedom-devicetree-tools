/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#include "linker_script.h"

LinkerScript::LinkerScript(const fdt &dtb, Memory rom_memory, Memory itim_memory,
                           Memory data_memory, Memory rodata_memory)
  : rom_memory(rom_memory), itim_memory(itim_memory),
    data_memory(data_memory), rodata_memory(rodata_memory)
{
}

string LinkerScript::describe() {
  string description = "";

  description += "OUTPUT_ARCH(\"riscv\")\n\n";

  description += "ENTRY(_enter)\n\n";

  /* Describe Memories */
  description += "MEMORY\n{\n";
  description += "\t" + rom_memory.describe() + "\n";
  if (itim_memory != rom_memory) {
    description += "\t" + itim_memory.describe() + "\n";
  }
  if (data_memory != rom_memory && data_memory != itim_memory) {
    description += "\t" + data_memory.describe() + "\n";
  }
  if (rodata_memory != rom_memory && rodata_memory != itim_memory && rodata_memory != data_memory) {
    description += "\t" + rodata_memory.describe() + "\n";
  }
  description += "}\n\n";

  /* Describe Program Headers */
  description += "PHDRS\n{\n";
  for (auto it = program_headers.begin(); it != program_headers.end(); it++) {
    description += "\t" + (*it).name + " " + (*it).flags + ";\n";
  }
  description += "}\n\n";

  /* Describe Sections */
  description += "SECTIONS\n{\n";
  for (auto it = section_groups.begin(); it != section_groups.end(); it++) {
    description += (*it).describe();
  }
  description += "}\n\n";

  return description;
}

