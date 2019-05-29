/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __CONSTANTS_GROUP__H
#define __CONSTANTS_GROUP__H

#include <list>
#include <string>

#include <fdt.h++>

#include <memory.h>
#include <phdr.h>
#include <section_group.h>

using std::list;
using std::string;

class ConstantsGroup : public SectionGroup {
  public:
    ConstantsGroup(const fdt &dtb);

    void add_constant(string constant);

    string describe();
};

#endif /* __CONSTANTS_GROUP__H */

