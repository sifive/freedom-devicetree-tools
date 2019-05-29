/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __TEXT_GROUP__H
#define __TEXT_GROUP__H

#include <list>
#include <string>

#include <memory.h>
#include <phdr.h>
#include <section_group.h>

using std::list;
using std::string;

class TextGroup : public SectionGroup {
  public:
    TextGroup(Memory logical_memory, Phdr logical_header,
              Memory virtual_memory, Phdr virtual_header);
};

#endif /* __TEXT_GROUP__H */

