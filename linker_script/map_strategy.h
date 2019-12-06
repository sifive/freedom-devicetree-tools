/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __MAP_STRATEGY__H
#define __MAP_STRATEGY__H

#include <list>
#include <string>

#include <fdt.h++>

#include <linker_script.h>
#include <memory.h>

using std::list;
using std::string;

typedef enum {
  LINK_STRATEGY_DEFAULT,
  LINK_STRATEGY_SCRATCHPAD,
  LINK_STRATEGY_RAMRODATA,
} LinkStrategy;

class MapStrategy {
public:
  void print_chosen_strategy(string name, LinkStrategy layout, Memory ram,
                             Memory rom, Memory itim);

  void extract_node_props(Memory &m, const node &n, uint32_t index,
                          uint32_t offset);
  bool has_entry(const fdt &dtb);
  Memory get_entry_memory(const fdt &dtb);

  virtual bool valid(const fdt &dtb, list<Memory> available_memories) = 0;

  virtual LinkerScript create_layout(const fdt &dtb,
                                     list<Memory> available_memories,
                                     LinkStrategy link_strategy) = 0;
};

#endif /* __MAP_STRATEGY_H */
