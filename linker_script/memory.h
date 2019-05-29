/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __MEMORY__H
#define __MEMORY__H

#include <cstdint>
#include <list>
#include <string>

using std::list;
using std::string;

class Memory {
  public:
    /* Properties from DeviceTree */
    string compatible;
    uint64_t base;
    uint64_t size;

    /* Assigned by the MapStrategy */
    string name;
    string attributes;

    Memory();
    Memory(string compatible);

    Memory& operator=(const Memory& rhs);
    bool operator==(const Memory& rhs) const;
    bool operator!=(const Memory& rhs) const;
    bool operator< (const Memory& rhs) const;

    string describe();
};

#endif /* __MEMORY__H */

