/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __RANGES_H
#define __RANGES_H

#include <fdt.h++>

#include <list>

using std::list;

struct range_t {
  uint64_t
      child_address; /* physical address in the child's bus address space */
  uint64_t
      parent_address; /* physical address in the parent's bus address space */
  uint64_t size;      /* size of the range in the child's address space */

  range_t(uint64_t child, uint64_t parent, uint64_t size)
      : child_address(child), parent_address(parent), size(size) {}
};

typedef list<range_t> ranges_t;

ranges_t get_ranges(const node &n);

#endif /* __RANGES_H */
