/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#include "default_bullet_strategy.h"

#include <layouts/default_layout.h>
#include <layouts/scratchpad_layout.h>
#include <layouts/ramrodata_layout.h>

bool DefaultBulletStrategy::valid(const fdt &dtb, list<Memory> available_memories)
{
  bool testram = has_testram(available_memories);
  bool has_memory = false;

  /* Look through the available memories to determine if this is a valid strategy */
  for (auto it = available_memories.begin(); it != available_memories.end(); it++) {
    if ((*it).compatible == "memory") {
      has_memory = true;
    }
  }

  return (testram && has_memory);
}

LinkerScript DefaultBulletStrategy::create_layout(const fdt &dtb, list<Memory> available_memories,
                                             LinkStrategy link_strategy)
{
  Memory ram_memory;
  Memory itim_memory;
  bool has_itim = false;

  /* Map the available memories to the ROM, RAM, and ITIM */

  Memory rom_memory = find_testram(available_memories);
  rom_memory.name = "flash";
  rom_memory.attributes = "rxai!w";

  for (auto it = available_memories.begin(); it != available_memories.end(); it++) {
    if ((*it).compatible == "memory") {
      ram_memory = *it;
      ram_memory.name = "ram";
      ram_memory.attributes = "wxa!ri";
    } else if ((*it).compatible == "sifive,itim0" || it->compatible == "sifive,sram0") {
      itim_memory = *it;
      itim_memory.name = "itim";
      itim_memory.attributes = "wx!rai";

      has_itim = true;
    }
  }

  if (!has_itim) {
    itim_memory = ram_memory;
  }

  /* Generate the layouts */
  print_chosen_strategy("DefaultBulletStrategy", link_strategy, rom_memory, rom_memory, itim_memory);

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

