/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __SECTION_GROUP__H
#define __SECTION_GROUP__H

#include <list>
#include <string>

#include <memory.h>
#include <phdr.h>
#include <section.h>

using std::list;
using std::string;

class SectionGroup {
  public:
    list<Section> sections;

    Memory logical_memory;
    Phdr logical_header;
    Memory virtual_memory;
    Phdr virtual_header;

    list<string> leading_commands;
    list<string> trailing_commands;

    SectionGroup(Memory logical_memory, Phdr logical_header,
                 Memory virtual_memory, Phdr virtual_header);

    virtual string describe();
};

#endif /* __SECTION_GROUP__H */

