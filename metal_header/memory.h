/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_MEMORY__H
#define __METAL_HEADER_MEMORY__H

#include <device.h>

#include <regex>
#include <string>

using std::string;

class memory : public Device {
public:
  int num_memories = 0;

  memory(std::ostream &os, const fdt &dtb);
  void include_headers();
  void declare_structs();
  void define_structs();
  void create_handles();
};

#endif /* __METAL_HEADER_MEMORY__H */
