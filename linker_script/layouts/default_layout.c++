/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#include "default_layout.h"

#include <sections/constants_group.h>
#include <sections/ctors_group.h>
#include <sections/data_group.h>
#include <sections/init_text_group.h>
#include <sections/itim_group.h>
#include <sections/rodata_group.h>
#include <sections/text_group.h>
#include <sections/uninit_group.h>

DefaultLayout::DefaultLayout(const fdt &dtb, Memory rom_memory, Memory itim_memory,
                             Memory data_memory, Memory rodata_memory)
  : LinkerScript(dtb, rom_memory, itim_memory, data_memory, rodata_memory)
{
  Phdr rom_phdr("flash", "PT_LOAD");
  program_headers.push_back(rom_phdr);

  Phdr ram_phdr("ram", "PT_LOAD");
  Phdr ram_init_phdr("ram_init", "PT_LOAD");
  program_headers.push_back(ram_phdr);
  program_headers.push_back(ram_init_phdr);

  Phdr itim_phdr("itim", "PT_LOAD");
  Phdr itim_init_phdr("itim_init", "PT_LOAD");
  program_headers.push_back(itim_phdr);
  program_headers.push_back(itim_init_phdr);

  section_groups.push_back(ConstantsGroup(dtb));

  section_groups.push_back(InitTextGroup(rom_memory, rom_phdr,   /* Text sections are loaded from ROM */
                                         rom_memory, rom_phdr)); /* And stay in ROM when mapped */

  section_groups.push_back(TextGroup(rom_memory, rom_phdr,   /* Text sections are loaded from ROM */
                                     rom_memory, rom_phdr)); /* And stay in ROM when mapped */

  section_groups.push_back(RodataGroup(rom_memory, rom_phdr,   /* Read-only data is loaded from ROM */
                                       rom_memory, rom_phdr)); /* And stays in ROM when mapped */

  section_groups.push_back(CtorsGroup(rom_memory, rom_phdr,   /* Constructors are loaded from ROM */
                                      rom_memory, rom_phdr)); /* And stay in ROM when mapped */

  section_groups.push_back(ItimGroup(rom_memory, rom_phdr,          /* ITIM code is loaded from ROM */
                                     itim_memory, itim_init_phdr)); /* And is copied into the ITIM at load */

  section_groups.push_back(DataGroup(rom_memory, rom_phdr,         /* Data is loaded from ROM */
                                     data_memory, ram_init_phdr)); /* And copied into the data memory at load */

  section_groups.push_back(UninitGroup(dtb, data_memory, ram_phdr,   /* BSS and unitialized data isn't loaded from ROM */
                                       data_memory, ram_phdr)); /* And should end up in the data memory */
}

