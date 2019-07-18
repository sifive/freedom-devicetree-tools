/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __LINKER_SCRIPT__H
#define __LINKER_SCRIPT__H

#include <list>
#include <string>

#include <fdt.h++>

#include <section_group.h>

using std::list;
using std::string;

class LinkerScript {
  public:
    list<SectionGroup> section_groups;
    list<Phdr> program_headers;
    
    Memory rom_memory;
    Memory itim_memory;
    Memory data_memory;
    Memory rodata_memory;

    LinkerScript(const fdt &dtb, Memory rom_memory, Memory itim_memory,
                 Memory data_memory, Memory rodata_memory);

    virtual string describe();
};

#endif /* __LINKER_SCRIPT_H */

