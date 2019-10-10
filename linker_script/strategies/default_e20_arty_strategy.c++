/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#include "default_e20_arty_strategy.h"

#include <layouts/default_layout.h>
#include <layouts/scratchpad_layout.h>
#include <layouts/ramrodata_layout.h>

bool DefaultE20ArtyStrategy::valid(const fdt &dtb, list<Memory> available_memories)
{
  bool has_sram = false;
  bool has_spi = false;

  /* Look through the available memories to determine if this is a valid strategy */
  for (auto it = available_memories.begin(); it != available_memories.end(); it++) {
    if ((*it).compatible == "sifive,sram0") {
      has_sram = true;
    } else if ((*it).compatible == "sifive,spi0") {
      has_spi = true;
    }
  }

  return (has_sram && has_spi);
}

LinkerScript DefaultE20ArtyStrategy::create_layout(const fdt &dtb, list<Memory> available_memories,
                                             LinkStrategy link_strategy)
{
  Memory rom_memory;
  Memory ram_memory;
  Memory itim_memory;
  int sram0_count = 0;
  bool ram_mapped = false;
  bool has_itim = false;

  /* Map the available memories to the ROM, RAM, and ITIM */

  /* Count how many sram0, E20 typically only have one. Just in case */
  for (auto it = available_memories.begin(); it != available_memories.end(); it++) {
    if ((*it).compatible == "sifive,sram0") {
      sram0_count++;
    }
  }

  for (auto it = available_memories.begin(); it != available_memories.end(); it++) {
    if ((*it).compatible == "sifive,sram0") {
      if (sram0_count > 1) {
        /* Memories are presented in reverse order sorted by base address.
         * Therefore, the ram is mapped to the lower-based sram0 and the
         * itim is mapped to the higher-based sram0
         */
        if (ram_mapped) {
          has_itim = true;
          itim_memory = *it;
          itim_memory.name = "itim";
          itim_memory.attributes = "wx!rai";
        } else {
          ram_memory = *it;
          ram_memory.name = "ram";
          ram_memory.attributes = "wxa!ri";
          ram_mapped = true;
        }
      } else {
        /* Only one sram0, let be creative if we have enough space */
        if ((*it).size >= 0x10000) {
          ram_memory = *it;
          ram_memory.size = (*it).size / 2 - 0x1000;
          ram_memory.name = "ram";
          ram_memory.attributes = "wxa!ri";
          has_itim = true;
          itim_memory = *it;
          itim_memory.size = (*it).size / 2 + 0x1000;
          itim_memory.base = (*it).base + ram_memory.size;
          itim_memory.name = "itim";
          itim_memory.attributes = "wx!rai";
        } else {
          ram_memory = *it;
          ram_memory.name = "ram";
          ram_memory.attributes = "wxa!ri";
        }
      }
    } else if ((*it).compatible == "sifive,spi0") {
      rom_memory = *it;
      rom_memory.name = "flash";
      rom_memory.attributes = "rxai!w";
    }
  }

  /* Generate the layouts */
  print_chosen_strategy("DefaultE20ArtyStrategy", link_strategy, ram_memory, rom_memory, itim_memory);

  switch (link_strategy) {
  default:
  case LINK_STRATEGY_DEFAULT:
    if (has_itim) {
      return DefaultLayout(dtb, rom_memory, itim_memory, ram_memory, ram_memory);
    } else {
      return DefaultLayout(dtb, rom_memory, ram_memory, ram_memory, ram_memory);
    }
    break;

  case LINK_STRATEGY_SCRATCHPAD:
    return ScratchpadLayout(dtb, ram_memory, ram_memory, ram_memory,
                            ram_memory);
    break;

  case LINK_STRATEGY_RAMRODATA:
    if (has_itim) {
      return RamrodataLayout(dtb, rom_memory, itim_memory, ram_memory,
                             rom_memory);
    } else {
      return RamrodataLayout(dtb, rom_memory, ram_memory, ram_memory,
                             rom_memory);
    }
    break;
  }
}

