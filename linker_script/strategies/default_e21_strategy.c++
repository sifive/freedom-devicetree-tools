/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#include "default_e21_strategy.h"

#include <layouts/default_layout.h>
#include <layouts/scratchpad_layout.h>
#include <layouts/ramrodata_layout.h>

bool DefaultE21Strategy::valid(const fdt &dtb, list<Memory> available_memories)
{
  int sram_count = 0;
  bool testram = has_testram(available_memories);

  /* Look through the available memories to determine if this is a valid strategy */
  for (auto it = available_memories.begin(); it != available_memories.end(); it++) {
    if ((*it).compatible == "sifive,sram0") {
      sram_count += 1;
    }
  }

  return ((sram_count == 2) && testram);
}

LinkerScript DefaultE21Strategy::create_layout(const fdt &dtb, list<Memory> available_memories,
                                             LinkStrategy link_strategy)
{
  Memory rom_memory = find_testram(available_memories);
  rom_memory.name = "flash";
  rom_memory.attributes = "rxai!w";

  bool ram_mapped = false;
  Memory ram_memory;
  Memory itim_memory;

  /* Map the available memories to the ROM, RAM, and ITIM */

  for (auto it = available_memories.begin(); it != available_memories.end(); it++) {
    if ((*it).compatible == "sifive,sram0") {
      /* Memories are presented in reverse order sorted by base address.
       * Therefore, the ram is mapped to the lower-based sram0 and the
       * itim is mapped to the higher-based sram0
       */
      if (ram_mapped) {
        itim_memory = *it;
        itim_memory.name = "itim";
        itim_memory.attributes = "wx!rai";
      } else {
        ram_memory = *it;
        ram_memory.name = "ram";
        ram_memory.attributes = "wxa!ri";
        ram_mapped = true;
      }
    }
  }

  /* Generate the layouts */
  print_chosen_strategy("DefaultE21Strategy", link_strategy, ram_memory, rom_memory, itim_memory);

  switch (link_strategy) {
  default:
  case LINK_STRATEGY_DEFAULT:
    return DefaultLayout(dtb, rom_memory, itim_memory, ram_memory, ram_memory);
    break;

  case LINK_STRATEGY_SCRATCHPAD:
    return ScratchpadLayout(dtb, ram_memory, itim_memory, ram_memory, ram_memory);
    break;

  case LINK_STRATEGY_RAMRODATA:
    return RamrodataLayout(dtb, rom_memory, itim_memory, ram_memory, rom_memory);
    break;
  }
}

