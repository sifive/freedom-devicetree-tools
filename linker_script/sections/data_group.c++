/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#include "data_group.h"

DataGroup::DataGroup(Memory logical_memory, Phdr logical_header,
                     Memory virtual_memory, Phdr virtual_header)
  : SectionGroup(logical_memory, logical_header,
                 virtual_memory, virtual_header)
{
  Section lalign(logical_memory, logical_memory, logical_header);

  lalign.output_name = "lalign";

  lalign.add_command(". = ALIGN(8);");
  lalign.add_command("PROVIDE( _data_lma = . );");
  lalign.add_command("PROVIDE( metal_segment_data_source_start = . );");

  sections.push_back(lalign);

  Section dalign(logical_memory, virtual_memory, virtual_header);

  dalign.output_name = "dalign";

  dalign.add_command(". = ALIGN(8);");
  dalign.add_command("PROVIDE( metal_segment_data_target_start = . );");

  sections.push_back(dalign);

  Section data(logical_memory, virtual_memory, virtual_header);

  data.output_name = "data";

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

  sections.push_back(data);

  trailing_commands.push_back("PROVIDE( _edata = . );");
  trailing_commands.push_back("PROVIDE( edata = . );");
  trailing_commands.push_back("PROVIDE( metal_segment_data_target_end = . );");
}


