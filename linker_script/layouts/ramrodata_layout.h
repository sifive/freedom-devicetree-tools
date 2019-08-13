/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __RAMRODATA_LAYOUT__H
#define __RAMRODATA_LAYOUT__H

#include <list>
#include <string>

#include <linker_script.h>

using std::list;
using std::string;

/*
 * MAGIC_RAMRODATA_TEXT_THRESHOLD sets the threshold size of the ITIM at or 
 * over which the "ramrodata" layout places the text section into the ITIM.
 */
#define MAGIC_RAMRODATA_TEXT_THRESHOLD 0x8000

class RamrodataLayout : public LinkerScript {
  public:
    RamrodataLayout(const fdt &dtb, Memory rom_memory, Memory itim_memory,
                    Memory data_memory, Memory rodata_memory);
};

#endif /* __RAMRODATA_LAYOUT_H */

