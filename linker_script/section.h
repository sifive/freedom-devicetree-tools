/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __SECTION__H
#define __SECTION__H

#include <list>
#include <string>

#include <memory.h>
#include <phdr.h>

using std::list;
using std::string;

class Section {
public:
  string output_name;
  int alignment;
  list<string> commands;
  list<string> trailing_commands;

    Memory logical_memory;
    Memory virtual_memory;
    Phdr program_header;

    Section(Memory logical_memory, Memory virtual_memory, Phdr program_header);

    void add_command(string command);
    void add_trailing_command(string command);

    virtual string describe();
};

#endif /* __SECTION__H */

