/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#include "default_e20_strategy.h"

#include <layouts/default_layout.h>
#include <layouts/scratchpad_layout.h>
#include <layouts/ramrodata_layout.h>

bool DefaultE20Strategy::valid(const fdt &dtb, list<Memory> available_memories)
{
  return has_testram(available_memories);
}

LinkerScript DefaultE20Strategy::create_layout(const fdt &dtb, list<Memory> available_memories,
                                             LinkStrategy link_strategy)
{
  Memory rom_memory = find_testram(available_memories);
  rom_memory.name = "ram";
  rom_memory.attributes = "wxa!ri";

  /* Generate the layouts */
  print_chosen_strategy("DefaultE20Strategy", link_strategy, rom_memory, rom_memory, rom_memory);

  switch (link_strategy) {
  default:
  case LINK_STRATEGY_DEFAULT:
    return DefaultLayout(dtb, rom_memory, rom_memory, rom_memory, rom_memory);
    break;

  case LINK_STRATEGY_SCRATCHPAD:
    return ScratchpadLayout(dtb, rom_memory, rom_memory, rom_memory, rom_memory);
    break;

  case LINK_STRATEGY_RAMRODATA:
    return RamrodataLayout(dtb, rom_memory, rom_memory, rom_memory, rom_memory);
    break;
  }
}

