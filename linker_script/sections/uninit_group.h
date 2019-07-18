/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __UNINIT_GROUP__H
#define __UNINIT_GROUP__H

#include <list>
#include <string>

#include <fdt.h++>

#include <memory.h>
#include <phdr.h>
#include <section_group.h>

using std::list;
using std::string;

class UninitGroup : public SectionGroup {
  public:
    UninitGroup(const fdt &dtb, Memory logical_memory, Phdr logical_header,
                Memory virtual_memory, Phdr virtual_header);
};

#endif /* __UNINIT_GROUP__H */

