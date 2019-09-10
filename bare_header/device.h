/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_DEVICE__H
#define __BARE_HEADER_DEVICE__H

#include "fdt.h++"
#include "libfdt.h++"

#include "header-labels.h"

#include <ostream>
#include <string>

using std::string;

class Device {
  public:
    std::ostream &os;

    /* The "compatable" string for the device */
    std::string compat_string;

    /* The devicetree */
    const fdt &dtb;

    Device(std::ostream &os, const fdt &dtb, std::string compat_string);

    virtual int get_index(const node &n);
    int get_index(const node &n, string compat);

    virtual void emit_defines() {}

    virtual void emit_offsets() {}

    void emit_comment(const node &n);

    string def_handle(const node &n);
    string def_handle_index(const node &n);
    virtual uint64_t base_address(const node &n);
    void emit_base(const node &n);

    virtual uint64_t size(const node &n);
    void emit_size(const node &n);

    void emit_compat(string custom_string);
    void emit_compat();

    void emit_offset(string offset_name, uint32_t offset);

    void emit_property_u32(const node &n, string property_name, uint32_t value);
};

#endif
