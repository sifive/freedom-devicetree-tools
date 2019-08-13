/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_STDOUT_PATH__H
#define __METAL_HEADER_STDOUT_PATH__H

#include <device.h>

#include <regex>

class stdout_path : public Device {
public:
  stdout_path(std::ostream &os, const fdt &dtb);
  void create_handles();
};

#endif
