/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#include "section_group.h"

SectionGroup::SectionGroup(Memory logical_memory, Phdr logical_header,
                           Memory virtual_memory, Phdr virtual_header)
  : logical_memory(logical_memory),
    logical_header(logical_header),
    virtual_memory(virtual_memory),
    virtual_header(virtual_header)
{
}

string SectionGroup::describe()
{
  string description = "";

  for (auto it = leading_commands.begin(); it != leading_commands.end(); it++) {
    description += "\t" + (*it) += "\n";
  }
  description += "\n";

  for (auto it = sections.begin(); it != sections.end(); it++) {
    description += (*it).describe() += "\n";
  }

  for (auto it = trailing_commands.begin(); it != trailing_commands.end(); it++) {
    description += "\t" + (*it) += "\n";
  }
  description += "\n";

  return description;
}

void SectionGroup::add_command(string command) {
  leading_commands.push_back(command);
}

void SectionGroup::add_trailing_command(string command) {
  trailing_commands.push_back(command);
}
