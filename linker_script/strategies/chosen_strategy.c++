/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#include "chosen_strategy.h"

#include <layouts/default_layout.h>
#include <layouts/ramrodata_layout.h>
#include <layouts/scratchpad_layout.h>

bool ChosenStrategy::valid(const fdt &dtb, list<Memory> available_memories) {
  bool chosen_ram = false;

  dtb.chosen("metal,ram", tuple_t<node, uint32_t>(),
             [&](node n, uint32_t idx) { chosen_ram = true; });

  return (chosen_ram && has_entry(dtb));
}

LinkerScript ChosenStrategy::create_layout(const fdt &dtb,
                                           list<Memory> available_memories,
                                           LinkStrategy link_strategy) {
  Memory rom_memory;
  Memory ram_memory;
  Memory itim_memory;
  bool has_itim = false;

  rom_memory = get_entry_memory(dtb);

  dtb.chosen("metal,ram", tuple_t<node, uint32_t>(), [&](node n, uint32_t idx) {
    ram_memory.name = "ram";
    extract_node_props(ram_memory, n, idx, 0);
    ram_memory.attributes = "wxa!ri";
  });
  dtb.chosen("metal,itim", tuple_t<node>(), [&](node n) {
    itim_memory.name = "itim";
    extract_node_props(itim_memory, n, 0, 0);
    itim_memory.attributes = "wx!rai";

    has_itim = true;
  });

  /* Generate the layouts */

  if (has_itim) {
    print_chosen_strategy("ChosenStrategy", link_strategy, ram_memory,
                          rom_memory, itim_memory);
  } else {
    print_chosen_strategy("ChosenStrategy", link_strategy, ram_memory,
                          rom_memory, ram_memory);
  }

  switch (link_strategy) {
  default:
  case LINK_STRATEGY_DEFAULT:
    if (has_itim) {
      return DefaultLayout(dtb, rom_memory, itim_memory, ram_memory,
                           ram_memory);
    } else {
      return DefaultLayout(dtb, rom_memory, rom_memory, ram_memory, ram_memory);
    }
    break;

  case LINK_STRATEGY_SCRATCHPAD:
    if (has_itim) {
      return ScratchpadLayout(dtb, ram_memory, itim_memory, ram_memory,
                              ram_memory);
    } else {
      return ScratchpadLayout(dtb, ram_memory, ram_memory, ram_memory,
                              ram_memory);
    }
    break;

  case LINK_STRATEGY_RAMRODATA:
    if (has_itim) {
      return RamrodataLayout(dtb, rom_memory, itim_memory, ram_memory,
                             rom_memory);
    } else {
      return RamrodataLayout(dtb, rom_memory, rom_memory, ram_memory,
                             rom_memory);
    }
    break;
  }
}
