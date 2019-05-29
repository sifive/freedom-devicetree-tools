/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __SCRATCHPAD_LAYOUT__H
#define __SCRATCHPAD_LAYOUT__H

#include <list>
#include <string>

#include <linker_script.h>

using std::list;
using std::string;

class ScratchpadLayout : public LinkerScript {
  public:
    ScratchpadLayout(const fdt &dtb, Memory rom_memory, Memory itim_memory,
                     Memory data_memory, Memory rodata_memory);
};

#endif /* __SCRATCHPAD_LAYOUT_H */

