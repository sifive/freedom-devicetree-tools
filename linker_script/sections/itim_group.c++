/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#include "itim_group.h"

ItimGroup::ItimGroup(Memory logical_memory, Phdr logical_header,
                     Memory virtual_memory, Phdr virtual_header)
  : SectionGroup(logical_memory, logical_header,
                 virtual_memory, virtual_header)
{
  Section itim_section(logical_memory, virtual_memory, virtual_header);

  itim_section.output_name = "itim";
  itim_section.alignment = 8;

  itim_section.add_command("*(.itim .itim.*)");

  sections.push_back(itim_section);

  trailing_commands.push_back("PROVIDE( metal_segment_itim_source_start = LOADADDR(.itim) );");
  trailing_commands.push_back("PROVIDE( metal_segment_itim_target_start = ADDR(.itim) );");
  trailing_commands.push_back("PROVIDE( metal_segment_itim_target_end = ADDR(.itim) + SIZEOF(.itim) );");
}

