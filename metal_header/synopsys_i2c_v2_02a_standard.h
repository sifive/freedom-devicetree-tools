/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */
#ifndef __METAL_HEADER_SYNOPSYS_I2C_V2_02A_STANDARD__H
#define __METAL_HEADER_SYNOPSYS_I2C_V2_02A_STANDARD__H

#include <device.h>

#include <regex>

class synopsys_i2c_v2_02a_standard : public Device {
public:
  int num_i2cs = 0;
  int max_interrupts = 0;

  synopsys_i2c_v2_02a_standard(std::ostream &os, const fdt &dtb);
  
  void create_defines();
  void include_headers();
  void declare_inlines();
  void define_inlines();
  void declare_structs();
  void define_structs();
  void create_handles();
};

#endif