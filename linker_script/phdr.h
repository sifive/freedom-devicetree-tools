/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __PHDR__H
#define __PHDR__H

#include <list>
#include <string>

using std::list;
using std::string;

class Phdr {
  public:
    string name;
    string flags;

    Phdr();
    Phdr(string name, string flags);
};

#endif /* __PHDR__H */

