/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_TEMPLATE_DEVICE__H
#define __METAL_HEADER_TEMPLATE_DEVICE__H

#include <device.h>

#include <regex>

class sifive_fu540_c000_l2 : public Device {
  public:
    sifive_fu540_c000_l2(std::ostream &os, const fdt &dtb);
    void include_headers();
    void declare_inlines();
    void define_inlines();
    void declare_structs();
    void define_structs();
    void create_handles();
};

#endif
