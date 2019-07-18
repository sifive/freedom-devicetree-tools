/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <regex>
#include <string>

#include "map_strategy.h"

using std::cerr;
using std::cout;
using std::dec;
using std::endl;
using std::hex;
using std::max;
using std::max_element;
using std::min;
using std::min_element;
using std::regex;
using std::setw;
using std::string;
using std::transform;

/* MEDLOW_MIN and MEDLOW_MAX are the inclusive extrema of the range of
 * addresses reachable with a signed 32-bit offset from address 0 */
#define MEDLOW_MIN ((uint64_t) INT32_MIN)
#define MEDLOW_MAX ((uint64_t) INT32_MAX)

/* MEDANY_RANGE is the maximum addressable distance between two symbols in the
 * medany code model outside of the medlow-addressable range */
#define MEDANY_RANGE ((uint64_t) INT32_MAX)

/* TestRam Compatible Strings
 *
 * Devices which match these strings are potential candidates for being the
 * reset vector in RTL simulation. They are kept in order corresponding to the
 * heuristic for reset vector selection
 */
static list<string> testram_compats = {
  "sifive,ahb-periph-port",
  "sifive,apb-periph-port",
  "sifive,axi4-periph-port",
  "sifive,tl-periph-port",
  "sifive,periph-port",
  "sifive,ahb-sys-port",
  "sifive,apb-sys-port",
  "sifive,axi4-sys-port",
  "sifive,sys-port",
  "sifive,tl-sys-port",
  "sifive,ahb-mem-port",
  "sifive,apb-mem-port",
  "sifive,axi4-mem-port",
  "sifive,tl-mem-port",
  "sifive,mem-port",
};

static int get_xlen(const fdt &dtb) {
  int boothartid = 0;
  dtb.chosen(
    "metal,boothart",
    tuple_t<node>(),
    [&](node n) {
      boothartid = std::stoi(n.instance());
    });

  node boothart = dtb.node_by_path("/cpus/cpu@" + std::to_string(boothartid));

  if (boothart.field_exists("riscv,isa")) {
    string isa = boothart.get_field<string>("riscv,isa");
    if(isa.find("rv32") != string::npos) {
      return 32;
    } else if (isa.find("rv64") != string::npos) {
      return 64;
    }
  }

  cerr << "ERROR: Unable to determine XLEN of target" << endl;
  exit(1);

  return 0;
}

static bool memory_in_medlow(Memory m) {
  const int64_t base = (int64_t)m.base;
  const int64_t top = ((int64_t)m.base + m.size - 1);

  return (base >= MEDLOW_MIN) && (top <= MEDLOW_MAX);
}

static bool check_base_addr_distance(Memory a, Memory b) {
  return ((max(a.base, b.base) - min(a.base, b.base)) < MEDANY_RANGE);
}

static void warn_medany_reachability(Memory a, Memory b) {
  const uint64_t base_a = max(a.base, MEDLOW_MAX + 1);
  const uint64_t base_b = max(b.base, MEDLOW_MAX + 1);

  const uint64_t top_a = min((uint64_t)((int64_t)a.base + a.size - 1), MEDLOW_MIN - 1);
  const uint64_t top_b = min((uint64_t)((int64_t)b.base + b.size - 1), MEDLOW_MIN - 1);

  if (a.base > b.base) {
    if ((top_a - base_b) > MEDANY_RANGE) {
      cerr << hex;
      cerr << "WARNING: not all of the memory in '" << a.compatible << "' @ 0x" \
           << a.base << " will be addressable with the medany code model" << endl;
      cerr << dec;
    }
  } else if (b.base > a.base){
    if ((top_b - base_a) > MEDANY_RANGE) {
      cerr << hex;
      cerr << "WARNING: not all of the memory in '" << b.compatible << "' @ 0x" \
           << b.base << " will be addressable with the medany code model" << endl;
      cerr << dec;
    }
  }
}

bool MapStrategy::check_cmodel_support(const fdt &dtb, CodeModel cmodel, Memory ram, Memory rom, Memory itim)
{
  /* Any 32-bit design should support any existing code model. */
  if (get_xlen(dtb) == 64) {
    bool rom_in_medlow = memory_in_medlow(rom);
    bool ram_in_medlow = memory_in_medlow(ram);
    bool itim_in_medlow = memory_in_medlow(itim);

    switch (cmodel) {
    default:
      return false;
    case CODE_MODEL_MEDLOW:
      return (rom_in_medlow && ram_in_medlow && itim_in_medlow);
    case CODE_MODEL_MEDANY:
        if (rom_in_medlow && ram_in_medlow && itim_in_medlow) {
          /* If the memories are all addressable my medlow, they're addressable
           * by medany */
          return true;

        } else if (ram_in_medlow) {
          if ( ! (rom_in_medlow && itim_in_medlow) ) {
            /* Print a warning if the entire rom and itim can't be used by
             * PC-relative addressing */
            warn_medany_reachability(rom, itim);
          }

          /* If the rom and/or the itim are not addressable by medlow, then
           * they just need to be close enough together for PC-relative
           * addressing */
          return check_base_addr_distance(rom, itim);

        } else {
          /* Print a warning if the entire ram can't be used by PC-relative
           * addressing */
          if (rom.base > itim.base) {
            warn_medany_reachability(ram, itim);
          } else {
            warn_medany_reachability(ram, rom);
          }

          /* If the ram is not addressable by medlow, then the ram needs to
           * be close enough to both the rom and the itim */
          return (check_base_addr_distance(ram, rom) && check_base_addr_distance(ram, itim));
        }
      break;
    }
  }

  return true;
}

void MapStrategy::check_all_cmodels(const fdt &dtb, Memory ram, Memory rom, Memory itim)
{
  bool supported = false;

  if (check_cmodel_support(dtb, CODE_MODEL_MEDLOW, ram, rom, itim)) {
    supported = true;
  } else {
    cerr << "WARNING: the 'medlow' code model is not supported on this target" << endl;
  }

  if (check_cmodel_support(dtb, CODE_MODEL_MEDANY, ram, rom, itim)) {
    supported = true;
  } else {
    cerr << "WARNING: the 'medany' code model is not supported on this target" << endl;
  }

  if (!supported) {
    cerr << "ERROR: no code models are supported on this target" << endl;
    cerr << "ERROR: the memory map must be changed to support building C code" << endl;
    exit(1);
  }
}

void MapStrategy::print_chosen_strategy(const fdt &dtb, string name,
                                        LinkStrategy layout,
                                        Memory ram, Memory rom, Memory itim)
{
  cout << hex;
  cout << "Using strategy " << name;
  cout << " with ";
  switch(layout) {
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
  cout << "\tRAM:  " << setw(25) << ram.compatible << " - 0x" << ram.base << endl;
  cout << "\tROM:  " << setw(25) << rom.compatible << " - 0x" << rom.base << endl;
  cout << "\tITIM: " << setw(25) << itim.compatible << " - 0x" << itim.base << endl;
  cout << dec;

  /* We've identified the memories to be used and are telling the user about it.
   * Now seems like a good time to warn them about code model support and crash
   * if none are supported. */
  check_all_cmodels(dtb, ram, rom, itim);
}

bool MapStrategy::has_testram(list<Memory> memories) {
  memories.sort();

  for (auto mem = memories.begin(); mem != memories.end(); mem++) {
    for (auto c = testram_compats.begin(); c != testram_compats.end(); c++) {
      if (mem->compatible == *c) {
        return true;
      }
    }
  }

  return false;
}

Memory MapStrategy::find_testram(list<Memory> memories) {
  Memory testram;

  memories.sort();

  for (auto c = testram_compats.begin(); c != testram_compats.end(); c++) {
    for (auto mem = memories.begin(); mem != memories.end(); mem++) {
      if (mem->compatible == *c) {
        return *mem;
      }
    }
  }

  return testram;
}

