/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __REGS_H
#define __REGS_H

#include <fdt.h++>

#include <list>

using std::list;

struct reg_t {
  uint64_t address; /* bus address space */
  uint64_t size;    /* size of the  address space */

  reg_t(uint64_t address, uint64_t size) : address(address), size(size) {}
};

typedef list<reg_t> regs_t;

regs_t get_regs(const node &n);

#endif /* __REGS_H */
