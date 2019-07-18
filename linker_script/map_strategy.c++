/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#include <iostream>
#include <iomanip>
#include <string>
#include <regex>

#include "map_strategy.h"

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

void MapStrategy::print_chosen_strategy(string name, LinkStrategy layout,
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

