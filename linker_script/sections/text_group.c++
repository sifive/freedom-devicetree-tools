/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#include "text_group.h"

TextGroup::TextGroup(Memory logical_memory, Phdr logical_header,
                     Memory virtual_memory, Phdr virtual_header)
  : SectionGroup(logical_memory, logical_header,
                 virtual_memory, virtual_header)
{
  Section init_section(logical_memory, virtual_memory, logical_header);

  init_section.output_name = "init";

  init_section.add_command("KEEP (*(.text.metal.init.enter))");
  init_section.add_command("KEEP (*(SORT_NONE(.init)))");
  init_section.add_command("KEEP (*(.text.libgloss.start))");

  sections.push_back(init_section);

  Section text_section(logical_memory, virtual_memory, logical_header);

  text_section.output_name = "text";

  text_section.add_command("*(.text.unlikely .text.unlikely.*)");
  text_section.add_command("*(.text.startup .text.startup.*)");
  text_section.add_command("*(.text .text.*)");
  text_section.add_command("*(.gnu.linkonce.t.*)");

  sections.push_back(text_section);

  Section fini_section(logical_memory, virtual_memory, logical_header);

  fini_section.output_name = "fini";

  fini_section.add_command("KEEP (*(SORT_NONE(.fini)))");

  sections.push_back(fini_section);

  trailing_commands.push_back("PROVIDE (__etext = .);");
  trailing_commands.push_back("PROVIDE (_etext = .);");
  trailing_commands.push_back("PROVIDE (etext = .);");
}

