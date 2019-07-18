/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#include "ctors_group.h"

CtorsGroup::CtorsGroup(Memory logical_memory, Phdr logical_header,
                     Memory virtual_memory, Phdr virtual_header)
  : SectionGroup(logical_memory, logical_header,
                 virtual_memory, virtual_header)
{
  leading_commands.push_back(". = ALIGN(8);");

  Section preinit_array(logical_memory, virtual_memory, logical_header);

  preinit_array.output_name = "preinit_array";

  preinit_array.add_command("PROVIDE_HIDDEN (__preinit_array_start = .);");
  preinit_array.add_command("KEEP (*(.preinit_array))");
  preinit_array.add_command("PROVIDE_HIDDEN (__preinit_array_end = .);");

  sections.push_back(preinit_array);

  Section init_array(logical_memory, virtual_memory, logical_header);

  init_array.output_name = "init_array";

  init_array.add_command("PROVIDE_HIDDEN (__init_array_start = .);");
  init_array.add_command("KEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))");
  init_array.add_command("KEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))");
  init_array.add_command("PROVIDE_HIDDEN (__init_array_end = .);");

  sections.push_back(init_array);

  Section fini_array(logical_memory, virtual_memory, logical_header);

  fini_array.output_name = "fini_array";

  fini_array.add_command("PROVIDE_HIDDEN (__fini_array_start = .);");
  fini_array.add_command("KEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))");
  fini_array.add_command("KEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))");
  fini_array.add_command("PROVIDE_HIDDEN (__fini_array_end = .);");

  sections.push_back(fini_array);

  Section ctors(logical_memory, virtual_memory, logical_header);

  ctors.output_name = "ctors";

  ctors.add_command("KEEP (*crtbegin.o(.ctors))");
  ctors.add_command("KEEP (*crtbegin?.o(.ctors))");
  ctors.add_command("KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))");
  ctors.add_command("KEEP (*(SORT(.ctors.*)))");
  ctors.add_command("KEEP (*(.ctors))");

  sections.push_back(ctors);

  Section dtors(logical_memory, virtual_memory, logical_header);

  dtors.output_name = "dtors";

  dtors.add_command("KEEP (*crtbegin.o(.dtors))");
  dtors.add_command("KEEP (*crtbegin?.o(.dtors))");
  dtors.add_command("KEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))");
  dtors.add_command("KEEP (*(SORT(.dtors.*)))");
  dtors.add_command("KEEP (*(.dtors))");

  sections.push_back(dtors);
}

