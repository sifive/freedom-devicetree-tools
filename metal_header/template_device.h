/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

/****************************************

  DEVICE TEMPLATE

  This file is a template for adding support for new hardware to Freedom Metal.

  To add a new device, create a copy of this file and rename all instances of
  "template_device" to the name of the device you're adding. Make sure you
  remember to change the #ifndef at the top too!

****************************************/

#ifndef __METAL_HEADER_TEMPLATE_DEVICE__H
#define __METAL_HEADER_TEMPLATE_DEVICE__H

#include <device.h>

#include <regex>

class template_device : public Device {
public:
  template_device(std::ostream &os, const fdt &dtb);

  void create_machine_macros();
  void create_defines();
  void include_headers();
  void declare_inlines();
  void define_inlines();
  void declare_structs();
  void define_structs();
  void create_handles();
};

#endif
