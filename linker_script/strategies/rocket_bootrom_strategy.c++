/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#include "rocket_bootrom_strategy.h"

#include <layouts/default_layout.h>
#include <layouts/scratchpad_layout.h>
#include <layouts/ramrodata_layout.h>

bool RocketBootromStrategy::valid(const fdt &dtb, list<Memory> available_memories)
{
  bool has_dtim = false;
  bool has_bootrom = false;
  /* ITIM is optional */

  for (auto it = available_memories.begin(); it != available_memories.end(); it++) {
    if (it->compatible == "memory") {
      has_dtim = true;
    }
    if (it->compatible == "sifive,rom0") {
      has_bootrom = true;
    }
  }

  return (has_bootrom && has_dtim);
}

LinkerScript RocketBootromStrategy::create_layout(const fdt &dtb, list<Memory> available_memories,
                                                  LinkStrategy link_strategy)
{
  Memory rom_memory;
  Memory ram_memory;
  Memory itim_memory;
  bool has_itim = false;

  for (auto it = available_memories.begin(); it != available_memories.end(); it++) {
    if (it->compatible == "sifive,rom0") {
      rom_memory = *it;
      rom_memory.name = "flash";
      rom_memory.attributes = "rxai!w";
    } else if (it->compatible == "memory") {
      ram_memory = *it;
      ram_memory.name = "ram";
      ram_memory.attributes = "wxa!ri";
    } else if (it->compatible == "sifive,itim0") {
      has_itim = true;
      itim_memory = *it;
      itim_memory.name = "itim";
      itim_memory.attributes = "wx!rai";
    }
  }

  if (!has_itim) {
    itim_memory = ram_memory;
  }

  print_chosen_strategy("RocketBootromStrategy", link_strategy, ram_memory, rom_memory, itim_memory);

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

