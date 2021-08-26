/* Copyright 2021 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_SIFIVE_TWINJET__H
#define __METAL_HEADER_SIFIVE_TWINJET__H

#include <device.h>

#include <regex>

class sifive_twinjet : public Device {
private:
  bool is_present;

public:
  sifive_twinjet(std::ostream &os, const fdt &dtb);
  void include_headers();
  void declare_structs();
  void define_structs();
  void create_handles();
};

#endif /* __METAL_HEADER_SIFIVE_TWINJET__H */
