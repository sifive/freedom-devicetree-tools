/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#include <algorithm>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <ranges.h>
#include <regex>
#include <regs.h>
#include <string>

#include "map_strategy.h"

using std::cerr;
using std::cout;
using std::dec;
using std::endl;
using std::hex;
using std::regex;
using std::setw;
using std::string;

/* TestRam Compatible Strings
 *
 * Devices which match these strings are potential candidates for being the
 * reset vector in RTL simulation. They are kept in order corresponding to the
 * heuristic for reset vector selection
 */
static list<string> testram_compats = {
    "sifive,ahb-periph-port",  "sifive,apb-periph-port",
    "sifive,axi4-periph-port", "sifive,tl-periph-port",
    "sifive,periph-port",      "sifive,ahb-sys-port",
    "sifive,apb-sys-port",     "sifive,axi4-sys-port",
    "sifive,sys-port",         "sifive,tl-sys-port",
    "sifive,inter-sys-port",   "sifive,tl-inter-sys-port",
    "sifive,ahb-mem-port",     "sifive,apb-mem-port",
    "sifive,axi4-mem-port",    "sifive,tl-mem-port",
    "sifive,mem-port",
};

void MapStrategy::extract_node_props(Memory &m, const node &n, uint32_t index,
                                     uint32_t offset) {
  if (n.field_exists("reg-names")) {
    n.named_tuples("reg-names", "reg", "mem",
                   tuple_t<target_addr, target_size>(),
                   [&](target_addr b, target_size s) {
                     m.base = b + offset;
                     m.size = s;
                   });
  } else if (n.field_exists("ranges")) {
    ranges_t ranges = get_ranges(n);

    if (index < ranges.size()) {
      auto range = std::next(ranges.begin(), index);
      if (offset > range->size) {
        cerr << "Requested offset is " << offset
             << " but the size of the address range is " << range->size << endl;
        std::exit(1);
      }
      m.base = range->parent_address + offset;
      m.size = range->size;
    } else {
      cerr << "Requested index is " << index
           << " but highest available tuple index is " << ranges.size() << endl;
      std::exit(1);
    }
  } else if (n.field_exists("reg")) {
    regs_t regs = get_regs(n);

    if (index < regs.size()) {
      auto reg = std::next(regs.begin(), index);
      if (offset > reg->size) {
        cerr << "Requested offset is " << offset
             << " but the size of the address range is " << reg->size << endl;
        std::exit(1);
      }
      m.base = reg->address + offset;
      m.size = reg->size;
    } else {
      cerr << "Requested index is " << index
           << " but highest available tuple index is " << regs.size() << endl;
      std::exit(1);
    }
  }

  n.maybe_tuple("compatible", tuple_t<string>(), [&]() {},
                [&](string compat) { m.compatible = compat; });
}

void MapStrategy::print_chosen_strategy(string name, LinkStrategy layout,
                                        Memory ram, Memory rom, Memory itim) {
  cout << hex;
  cout << "Using strategy " << name;
  cout << " with ";
  switch (layout) {
  default:
  case LINK_STRATEGY_DEFAULT:
    cout << "default";
    break;
  case LINK_STRATEGY_SCRATCHPAD:
    cout << "scratchpad";
    break;
  case LINK_STRATEGY_RAMRODATA:
    cout << "ramrodata";
    break;
  }
  cout << " layout" << endl;
  cout << "\tRAM:  " << setw(25) << ram.compatible << " - 0x" << ram.base
       << endl;
  cout << "\tROM:  " << setw(25) << rom.compatible << " - 0x" << rom.base
       << endl;
  if (itim.base != ram.base) {
    cout << "\tITIM: " << setw(25) << itim.compatible << " - 0x" << itim.base
         << endl;
  }
  cout << dec;
}

bool MapStrategy::has_entry(const fdt &dtb) {
  bool chosen_rom = false;
  bool chosen_entry = false;
  dtb.chosen("metal,rom", tuple_t<node, uint32_t, uint32_t>(),
             [&](node n, uint32_t idx, uint32_t offset) { chosen_rom = true; });
  dtb.chosen("metal,entry", tuple_t<node, uint32_t, uint32_t>(),
             [&](node n, uint32_t idx, uint32_t offset) { chosen_rom = true; });
  return (chosen_rom || chosen_entry);
}

Memory MapStrategy::get_entry_memory(const fdt &dtb) {
  Memory rom_memory;
  bool rom_mapped = false;

  dtb.chosen("metal,entry", tuple_t<node, uint32_t, uint32_t>(),
             [&](node n, uint32_t idx, uint32_t offset) {
               rom_memory.name = "flash";
               extract_node_props(rom_memory, n, idx, offset);
               rom_memory.attributes = "rxai!w";
               rom_mapped = true;
             });
  if (!rom_mapped) { /* metal,rom is synonymous with metal,entry */
    dtb.chosen("metal,rom", tuple_t<node, uint32_t, uint32_t>(),
               [&](node n, uint32_t idx, uint32_t offset) {
                 rom_memory.name = "flash";
                 extract_node_props(rom_memory, n, idx, offset);
                 rom_memory.attributes = "rxai!w";
               });
  }

  return rom_memory;
}
