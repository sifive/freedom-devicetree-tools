/* Copyright 2021 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_MALLARD0__H
#define __METAL_HEADER_SIFIVE_MALLARD0__H

#include <device.h>

#include <regex>

class sifive_mallard0 : public Device {
private:
  bool skip;

public:
  sifive_mallard0(std::ostream &os, const fdt &dtb);
  void include_headers();
  void create_defines();
};

#endif /* __METAL_HEADER_SIFIVE_MALLARD0__H */
