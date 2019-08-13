/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#include "itim_text_group.h"

ItimTextGroup::ItimTextGroup(Memory logical_memory, Phdr logical_header,
                             Memory virtual_memory, Phdr virtual_header)
    : SectionGroup(logical_memory, logical_header, virtual_memory,
                   virtual_header) {
  Section litimalign(logical_memory, logical_memory, logical_header);

  litimalign.output_name = "litimalign";

  litimalign.add_command(". = ALIGN(8);");
  litimalign.add_command("PROVIDE( metal_segment_itim_source_start = . );");

  sections.push_back(litimalign);

  Section ditimalign(logical_memory, virtual_memory, virtual_header);

  ditimalign.output_name = "ditimalign";

  ditimalign.add_command(". = ALIGN(8);");
  ditimalign.add_command("PROVIDE( metal_segment_itim_target_start = . );");

  sections.push_back(ditimalign);

  Section text_section(logical_memory, virtual_memory, virtual_header);

  text_section.output_name = "text";

  text_section.add_command("*(.text.unlikely .text.unlikely.*)");
  text_section.add_command("*(.text.startup .text.startup.*)");
  text_section.add_command("*(.text .text.*)");
  text_section.add_command("*(.gnu.linkonce.t.*)");

  sections.push_back(text_section);

  Section itim_section(logical_memory, virtual_memory, virtual_header);

  itim_section.output_name = "itim_section";

  itim_section.add_command("*(.itim .itim.*)");

  sections.push_back(itim_section);

  trailing_commands.push_back("PROVIDE( metal_segment_itim_target_end = . );");
}
