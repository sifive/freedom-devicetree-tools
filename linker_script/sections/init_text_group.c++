/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#include "init_text_group.h"

InitTextGroup::InitTextGroup(Memory logical_memory, Phdr logical_header,
                     Memory virtual_memory, Phdr virtual_header)
  : SectionGroup(logical_memory, logical_header,
                 virtual_memory, virtual_header)
{
  Section init_section(logical_memory, virtual_memory, logical_header);

  init_section.output_name = "init";

  init_section.add_command("KEEP (*(.text.metal.init.enter))");
  init_section.add_command("KEEP (*(.text.metal.init.*))");
  init_section.add_command("KEEP (*(SORT_NONE(.init)))");
  init_section.add_command("KEEP (*(.text.libgloss.start))");

  sections.push_back(init_section);

  Section fini_section(logical_memory, virtual_memory, logical_header);

  fini_section.output_name = "fini";

  fini_section.add_command("KEEP (*(SORT_NONE(.fini)))");

  sections.push_back(fini_section);
}

