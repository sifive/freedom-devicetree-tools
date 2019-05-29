/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __TEMPLATE_STRATEGY__H
#define __TEMPLATE_STRATEGY__H

#include <list>
#include <string>

#include <map_strategy.h>
#include <memory.h>
#include <linker_script.h>

using std::list;
using std::string;

class TemplateStrategy : public MapStrategy {
  public:

    virtual bool valid(const fdt &dtb, list<Memory> available_memories);

    virtual LinkerScript create_layout(const fdt &dtb, list<Memory> available_memories,
                                       LinkStrategy link_strategy);
};

#endif /* __TEMPLATE_STRATEGY__H */

