/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#include <regex>

using std::regex;

#include "uninit_group.h"

UninitGroup::UninitGroup(const fdt &dtb, Memory logical_memory,
                         Phdr logical_header, Memory virtual_memory,
                         Phdr virtual_header, Phdr tls_header)
    : SectionGroup(logical_memory, logical_header, virtual_memory,
                   virtual_header) {
  int num_harts = 0;
  dtb.match(regex("cpu"), [&](node n) { num_harts += 1; });

  Section tbss(virtual_memory, virtual_memory, virtual_header, tls_header);

  tbss.output_name = "tbss";

  tbss.add_command("*(.tbss .tbss.* .gnu.linkonce.tb.*)");
  tbss.add_command("*(.tcommon .tcommon.*)");
  tbss.add_command("PROVIDE( __tls_end = . );");

  tbss.trailing_commands.push_back("PROVIDE( __tbss_size = SIZEOF(.tbss));");
  tbss.trailing_commands.push_back(
      "PROVIDE( __tls_size = __tls_end - __tls_base );");

  sections.push_back(tbss);

  Section tbss_space(virtual_memory, virtual_memory, virtual_header);

  tbss_space.output_name = "tbss_space";

  tbss_space.add_command(". = . + __tbss_size;");

  sections.push_back(tbss_space);

  Section bss(virtual_memory, virtual_memory, virtual_header);

  bss.output_name = "bss";
  bss.alignment = 8;

  bss.add_command("*(.sbss*)");
  bss.add_command("*(.gnu.linkonce.sb.*)");
  bss.add_command("*(.bss .bss.*)");
  bss.add_command("*(.gnu.linkonce.b.*)");
  bss.add_command("*(COMMON)");

  bss.trailing_commands.push_back(
      "PROVIDE( metal_segment_bss_target_start = ADDR(.tbss) );");
  bss.trailing_commands.push_back(
      "PROVIDE( metal_segment_bss_target_end = . );");

  sections.push_back(bss);

  Section stack(virtual_memory, virtual_memory, virtual_header);

  stack.output_name = "stack";

  stack.add_command("PROVIDE(metal_segment_stack_begin = .);");
  stack.add_command(". += __stack_size;");
  stack.add_command("PROVIDE( _sp = . );");
  for (int i = 0; i < (num_harts - 1); i++) {
    stack.add_command(". += __stack_size;");
  }
  stack.add_command("PROVIDE(metal_segment_stack_end = .);");

  sections.push_back(stack);

  Section heap(virtual_memory, virtual_memory, virtual_header);

  heap.output_name = "heap";

  heap.add_command("PROVIDE( metal_segment_heap_target_start = . );");
  heap.add_command(". = DEFINED(__heap_max) ? MIN( LENGTH(" +
                   virtual_memory.name + ") - ( . - ORIGIN(" +
                   virtual_memory.name +
                   ")) , 0x10000000"
                   ") : __heap_size;");
  heap.add_command("PROVIDE( metal_segment_heap_target_end = . );");
  heap.add_command("PROVIDE( _heap_end = . );");

  sections.push_back(heap);
}
