/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#include "rodata_group.h"

RodataGroup::RodataGroup(Memory logical_memory, Phdr logical_header,
                     Memory virtual_memory, Phdr virtual_header)
  : SectionGroup(logical_memory, logical_header,
                 virtual_memory, virtual_header)
{
  Section rodata_section(logical_memory, virtual_memory, logical_header);
  
  rodata_section.output_name = "rodata";

  rodata_section.add_command("*(.rdata)");
  rodata_section.add_command("*(.rodata .rodata.*)");
  rodata_section.add_command("*(.gnu.linkonce.r.*)");
  rodata_section.add_command(". = ALIGN(8);");
  rodata_section.add_command("*(.srodata.cst16)");
  rodata_section.add_command("*(.srodata.cst8)");
  rodata_section.add_command("*(.srodata.cst4)");
  rodata_section.add_command("*(.srodata.cst2)");
  rodata_section.add_command("*(.srodata .srodata.*)");

  sections.push_back(rodata_section);
}

