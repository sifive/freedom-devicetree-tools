/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_DEVICE__H
#define __METAL_HEADER_DEVICE__H

#include "fdt.h++"
#include "libfdt.h++"

#include <ostream>
#include <string>

class Device {
  public:
    std::ostream &os;

    /* The "compatable" string for the device */
    std::string compat_string;

    /* The devicetree */
    const fdt &dtb;

    Device(std::ostream &os, const fdt &dtb, std::string compat_string);

    virtual void create_defines() = 0;
    virtual void include_headers() = 0;
    virtual void declare_structs() = 0;
    virtual void define_structs() = 0;
    virtual void create_handles() = 0;

    void emit_comment(const node &n);
    void emit_handle(std::string d, const node &n, std::string v);

    void emit_def_handle(std::string handle, const node &n, std::string field);

    void emit_def(std::string handle, std::string field);
    void emit_def_value(std::string name, const node &n, std::string field);

    void emit_include(std::string d);

    void emit_struct_pointer_begin(std::string type, std::string name, std::string ext);
    void emit_struct_pointer_element(std::string type, uint32_t id,
				     std::string field, std::string delimiter);
    void emit_struct_pointer_end(std::string empty);

    void emit_struct_decl(std::string type, const node &n);

    void emit_struct_begin(std::string type, const node &n);

    void emit_struct_field(std::string field, std::string value);
    void emit_struct_field_null(std::string field);
    void emit_struct_field_u32(std::string field, uint32_t value);
    void emit_struct_field_ta(std::string field, target_addr value);
    void emit_struct_field_ts(std::string field, target_size value);

    void emit_struct_container_node_and_array(int size,
					      std::string field1,
					      const node& c,
					      std::string subfield1,
					      std::string field2,
					      uint32_t elem);

    void emit_struct_field_array_elem(int idx, std::string field, uint32_t elem);
    void emit_struct_field_node(std::string field, const node& n, std::string subfield);

    void emit_struct_end(void);

    void emit_struct_array_def_begin(std::string type, std::string name, std::string size);
    void emit_struct_array_elem_node(const node& n);
};

#endif
