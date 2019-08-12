/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __ITIM_TEXT_GROUP__H
#define __ITIM_TEXT_GROUP__H

#include <list>
#include <string>

#include <memory.h>
#include <phdr.h>
#include <section_group.h>

using std::list;
using std::string;

class ItimTextGroup : public SectionGroup {
  public:
    ItimTextGroup(Memory logical_memory, Phdr logical_header,
                  Memory virtual_memory, Phdr virtual_header);
};

#endif /* __ITIM_TEXT_GROUP__H */

