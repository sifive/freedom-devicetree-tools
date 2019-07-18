/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#include "default_e21_arty_strategy.h"

#include <layouts/default_layout.h>
#include <layouts/scratchpad_layout.h>
#include <layouts/ramrodata_layout.h>

bool DefaultE21ArtyStrategy::valid(const fdt &dtb, list<Memory> available_memories)
{
  int sram_count = 0;
  bool has_spi = false;

  /* Look through the available memories to determine if this is a valid strategy */
  for (auto it = available_memories.begin(); it != available_memories.end(); it++) {
    if ((*it).compatible == "sifive,sram0") {
      sram_count += 1;
    } else if ((*it).compatible == "sifive,spi0") {
      has_spi = true;
    }
  }

  return ((sram_count == 2) && has_spi);
}

LinkerScript DefaultE21ArtyStrategy::create_layout(const fdt &dtb, list<Memory> available_memories,
                                             LinkStrategy link_strategy)
{
  Memory rom_memory;
  Memory ram_memory;
  Memory itim_memory;
  bool ram_mapped = false;

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
    } else if ((*it).compatible == "sifive,spi0") {
      rom_memory = *it;
      rom_memory.name = "flash";
      rom_memory.attributes = "rxai!w";
    }
  }

  /* Generate the layouts */
  print_chosen_strategy("DefaultE21ArtyStrategy", link_strategy, ram_memory, rom_memory, itim_memory);

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

