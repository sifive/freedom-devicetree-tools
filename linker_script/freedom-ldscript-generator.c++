/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <fstream>
#include <iostream>
#include <list>
#include <regex>
#include <tuple>

#include <fdt.h++>

#include <memory.h>
#include <ranges.h>
#include <strategies/chosen_strategy.h>
#include <strategies/default_bullet_arty.h>
#include <strategies/default_bullet_strategy.h>
#include <strategies/default_e20_arty_strategy.h>
#include <strategies/default_e20_strategy.h>
#include <strategies/default_e21_arty_strategy.h>
#include <strategies/default_e21_strategy.h>
#include <strategies/default_rocket_arty.h>
#include <strategies/default_rocket_strategy.h>
#include <strategies/memory_only_strategy.h>

using std::cerr;
using std::cout;
using std::endl;
using std::ofstream;
using std::string;

void show_usage(string name)
{
  std::cerr << "Usage: " << name << " <option(s)>\n"
            << "General Options:\n"
            << "    -h,--help                               Show this help message\n"
            << "Input/Output Options:\n"
            << "    -d,--dtb <eg. xxx.dtb>                  Specify fullpath to the DTB file\n"
            << "    -l,--linker <eg. zzz.lds>               Generate linker file to fullpath filename\n"
            << "Linker Script Generation Options:\n"
            << "    --scratchpad                            Execute codes from ram, -l option\n"
            << "    --ramrodata                             Rodata in ram data section, -l option\n"
            << endl;
}

int main (int argc, char* argv[])
{
  /*
   * Parse Arguments
   */

  string dtb_file;
  string linker_file;
  string release;

  bool ramrodata = false;
  bool scratchpad = false;

  if ((argc < 2) || (argc > 7)) {
    show_usage(argv[0]);
    return 1;
  } else {
    for (int i = 1; i < argc; ++i) {
      string arg = argv[i];

      if ((arg == "-d") || (arg == "--dtb")) {
        if (i + 1 < argc) {
          dtb_file = argv[++i];
        } else {
          std::cerr << "--dtb option requires file path." << std::endl;
          show_usage(argv[0]);
          return 1;
        }
      } else if ((arg == "-l") || (arg == "--linker")) {
        if (i + 1 < argc) {
          linker_file = argv[++i];
        } else {
          std::cerr << "--linker option requires file path." << std::endl;
          show_usage(argv[0]);
          return 1;
        }
      } else if (arg == "--ramrodata") {
        ramrodata = true;
      } else if (arg == "--scratchpad") {
        scratchpad = true;
      } else if (arg == "-r") {
        if (i + 1 < argc) {
          release = argv[++i];
        }
      } else {
        show_usage(argv[0]);
        return 1;
      }
    }
  }

  if (dtb_file.empty()) {
      std::cerr << "--dtb option requires file path." << std::endl;
      show_usage(argv[0]);
      return 1;
  }
  fdt dtb(dtb_file);

  /*
   * Find memory devices
   */

  auto extract_mem_map = [](Memory &m, const node &n) {
    if(n.field_exists("reg-names")) {
      n.named_tuples(
        "reg-names",
        "reg", "mem",
        tuple_t<target_addr, target_size>(),
        [&](target_addr b, target_size s) {
          m.base = b;
          m.size = s;
        });
    } else if (n.field_exists("ranges")) {
      ranges_t ranges = get_ranges(n);

      /* TODO: How do we pick which of the ranges entries to use? */
      if (!ranges.empty()) {
        m.base = ranges.front().child_address;
        m.size = ranges.front().size;
      }
    } else {
      auto regs = n.get_fields<std::tuple<target_addr, target_size>>("reg");

      if (!regs.empty()) {
        m.base = std::get<0>(regs.front());
        m.size = std::get<1>(regs.front());
      }
    }
  };

  /* Memory Devices Compatible Strings
   *
   * These are the devices to extract from the DeviceTree to consider in
   * map strategies. The list is in no particular order (sorted alphabetically
   * for readability).
   *
   * A sister list is maintained in linker_script/map_strategy.c++ which
   * defines the set of devices which should be considered for reset vector
   * identification in RTL simulation.
   */
  list<string> memory_devices = {
    "memory",
    "sifive,ahb-mem-port",
    "sifive,ahb-periph-port",
    "sifive,ahb-sys-port",
    "sifive,apb-mem-port",
    "sifive,apb-periph-port",
    "sifive,apb-sys-port",
    "sifive,axi4-mem-port",
    "sifive,axi4-periph-port",
    "sifive,axi4-sys-port",
    "sifive,dtim0",
    "sifive,itim0",
    "sifive,mem-port",
    "sifive,periph-port",
    "sifive,spi0",
    "sifive,sram0",
    "sifive,sys-port",
    "sifive,tl-mem-port",
    "sifive,tl-periph-port",
    "sifive,tl-sys-port",
  };
  list<Memory> memories;

  /* Get the node pointed to by metal,entry */
  string entry_handle;
  uint32_t entry_offset = 0;;
  dtb.chosen(
    "metal,entry",
    tuple_t<node, uint32_t>(),
    [&](node n, uint32_t offset) {
      entry_handle = n.handle();
      entry_offset = offset;
    });

  for (auto it = memory_devices.begin(); it != memory_devices.end(); it++) {
    dtb.match(
        std::regex(*it),
        [&](const node n) {
          Memory mem(*it);
          extract_mem_map(mem, n);

          /* If we've requested an offset, add it to the node */
          if(n.handle() == entry_handle) {
            mem.base += entry_offset;
            mem.size -= entry_offset;
          }

          memories.push_back(mem);
        });
  }

  /* Sort memories (by base address) and then reverse so that the last
   * matching memory for each type is used by the Strategy*/
  memories.sort();
  memories.reverse();

  /*
   * Mapping Strategies
   *
   * Strategies are ordered from most to least complicated, because simpler
   * memory maps are more likely to match erroneously on more complicated
   * designs
   */

  list<MapStrategy *> strategies;

  /* The chosen node supersedes all default strategies */
  strategies.push_back(new ChosenStrategy());

  /* E21 is idiosyncratic (mapping two srams to dtim and itim) */
  strategies.push_back(new DefaultE21ArtyStrategy());
  strategies.push_back(new DefaultE21Strategy());

  /* Bullet and other targets prioritize the memory node */
  strategies.push_back(new DefaultBulletArtyStrategy());
  strategies.push_back(new DefaultBulletStrategy());

  /* Rocket is pretty straightforward */
  strategies.push_back(new DefaultRocketArtyStrategy());
  strategies.push_back(new DefaultRocketStrategy());

  /* E20 strategy goes after because it only uses one sram */
  strategies.push_back(new DefaultE20ArtyStrategy());
  strategies.push_back(new DefaultE20Strategy());

  /* If all else fails, just use the memory port (and optionally an itim) */
  strategies.push_back(new MemoryOnlyStrategy());

  ofstream os;
  os.open(linker_file);

  /*
   * Generate Linker Script
   *
   * Use the first valid strategy
   */
  bool strategy_found = false;
  for (auto it = strategies.begin(); it != strategies.end(); it++) {
    if ((*it)->valid(dtb ,memories)) {
      strategy_found = true;

      if (ramrodata) {
        LinkerScript lds = (*it)->create_layout(dtb, memories, LINK_STRATEGY_RAMRODATA);
        os << lds.describe();
      } else if (scratchpad) {
        LinkerScript lds = (*it)->create_layout(dtb, memories, LINK_STRATEGY_SCRATCHPAD);
        os << lds.describe();
      } else {
        LinkerScript lds = (*it)->create_layout(dtb, memories, LINK_STRATEGY_DEFAULT);
        os << lds.describe();
      }

      break;
    }
  }

  os.close();

  if (!strategy_found) {
    cerr << "No valid strategies found!";
    return 1;
  }

  return 0;
}
