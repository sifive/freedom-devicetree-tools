/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SYNOPSYS_I2S_SLAVE_V1_11A__H
#define __METAL_HEADER_SYNOPSYS_I2S_SLAVE_V1_11A__H

#include <device.h>

#include <regex>

class synopsys_i2s_slave_v1_11a : public Device {
public:
  int num_i2s = 0;
  int max_interrupts = 0;

  synopsys_i2s_slave_v1_11a (std::ostream &os, const fdt &dtb);
  
  void create_defines(); 
  void include_headers();
  void declare_inlines();
  void define_inlines();
  void declare_structs();
  void define_structs();
  void create_handles();
};

#endif
