/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#include "data_group.h"

DataGroup::DataGroup(Memory logical_memory, Phdr logical_header,
                     Memory virtual_memory, Phdr virtual_header)
    : SectionGroup(logical_memory, logical_header, virtual_memory,
                   virtual_header) {
  Section data(logical_memory, virtual_memory, virtual_header);

  data.output_name = "data";
  data.alignment = 8;

  data.add_command("*(.data .data.*)");
  data.add_command("*(.gnu.linkonce.d.*)");
  data.add_command(". = ALIGN(8);");
  data.add_command("PROVIDE( __global_pointer$ = . + 0x800 );");
  data.add_command("*(.sdata .sdata.* .sdata2.*)");
  data.add_command("*(.gnu.linkonce.s.*)");
  data.add_command(". = ALIGN(8);");
  data.add_command("*(.srodata.cst16)");
  data.add_command("*(.srodata.cst8)");
  data.add_command("*(.srodata.cst4)");
  data.add_command("*(.srodata.cst2)");
  data.add_command("*(.srodata .srodata.*)");
  data.add_command(". = ALIGN(8);");
  data.add_command("*(.rdata)");
  data.add_command("*(.rodata .rodata.*)");
  data.add_command("*(.gnu.linkonce.r.*)");

  sections.push_back(data);

  trailing_commands.push_back("PROVIDE( metal_segment_data_source_start = LOADADDR(.data) );");
  trailing_commands.push_back("PROVIDE( metal_segment_data_target_start = ADDR(.data) );");
  trailing_commands.push_back("PROVIDE( metal_segment_data_target_end = ADDR(.data) + SIZEOF(.data) );");
}


