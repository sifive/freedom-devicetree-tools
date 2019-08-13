/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#include "text_group.h"

TextGroup::TextGroup(Memory logical_memory, Phdr logical_header,
                     Memory virtual_memory, Phdr virtual_header)
  : SectionGroup(logical_memory, logical_header,
                 virtual_memory, virtual_header)
{
  Section text_section(logical_memory, virtual_memory, logical_header);

  text_section.output_name = "text";

  text_section.add_command("*(.text.unlikely .text.unlikely.*)");
  text_section.add_command("*(.text.startup .text.startup.*)");
  text_section.add_command("*(.text .text.*)");
  text_section.add_command("*(.gnu.linkonce.t.*)");

  sections.push_back(text_section);

  trailing_commands.push_back("PROVIDE (__etext = .);");
  trailing_commands.push_back("PROVIDE (_etext = .);");
  trailing_commands.push_back("PROVIDE (etext = .);");
}

