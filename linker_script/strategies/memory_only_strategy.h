/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __MEMORY_ONLY_STRATEGY__H
#define __MEMORY_ONLY_STRATEGY__H

#include <list>
#include <string>

#include <linker_script.h>
#include <map_strategy.h>
#include <memory.h>

using std::list;
using std::string;

class MemoryOnlyStrategy : public MapStrategy {
public:
  virtual bool valid(const fdt &dtb, list<Memory> available_memories);

  virtual LinkerScript create_layout(const fdt &dtb,
                                     list<Memory> available_memories,
                                     LinkStrategy link_strategy);
};

#endif /* __MEMORY_ONLY_STRATEGY__H */
