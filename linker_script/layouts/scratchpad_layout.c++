/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#include "scratchpad_layout.h"

#include <sections/constants_group.h>
#include <sections/ctors_group.h>
#include <sections/data_group.h>
#include <sections/init_text_group.h>
#include <sections/itim_group.h>
#include <sections/rodata_group.h>
#include <sections/text_group.h>
#include <sections/uninit_group.h>

ScratchpadLayout::ScratchpadLayout(const fdt &dtb, Memory rom_memory, Memory itim_memory,
                                   Memory data_memory, Memory rodata_memory)
  : LinkerScript(dtb, rom_memory, itim_memory, data_memory, rodata_memory)
{
  Phdr ram_phdr("ram", "PT_LOAD");
  Phdr ram_init_phdr("ram_init", "PT_LOAD");
  program_headers.push_back(ram_phdr);
  program_headers.push_back(ram_init_phdr);

  Phdr itim_phdr("itim", "PT_LOAD");
  Phdr itim_init_phdr("itim_init", "PT_LOAD");
  program_headers.push_back(itim_phdr);
  program_headers.push_back(itim_init_phdr);

  section_groups.push_back(ConstantsGroup(dtb));

  section_groups.push_back(InitTextGroup(data_memory, ram_phdr,
                                          data_memory, ram_phdr));

  section_groups.push_back(TextGroup(data_memory, ram_phdr,
                                     data_memory, ram_phdr));

  section_groups.push_back(RodataGroup(data_memory, ram_phdr,
                                       data_memory, ram_phdr));

  section_groups.push_back(CtorsGroup(data_memory, ram_phdr,
                                      data_memory, ram_phdr));

  section_groups.push_back(ItimGroup(data_memory, ram_phdr,         /* ITIM code exists in the ROM, located in the data memory */
                                     itim_memory, itim_init_phdr)); /* And is copied into the ITIM at program load */

  section_groups.push_back(DataGroup(data_memory, ram_phdr,
                                     data_memory, ram_init_phdr));

  section_groups.push_back(UninitGroup(dtb, data_memory, ram_phdr,
                                       data_memory, ram_phdr));
}

