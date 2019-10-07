/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __METAL_HEADER_DEVICE__H
#define __METAL_HEADER_DEVICE__H

#include "fdt.h++"
#include "libfdt.h++"
#include "metal_header/inline.h"
#include <stdarg.h> /* va_list, va_start, va_arg, va_end */

#include "header-labels.h"

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

  virtual void create_machine_macros() {}
  virtual void create_defines() {}
  virtual void include_headers() {}
  virtual void declare_inlines() {}
  virtual void define_inlines() {}
  virtual void declare_structs() {}
  virtual void define_structs() {}
  virtual void create_handles() {}

  void emit_comment(const node &n);
  void emit_handle(std::string d, const node &n, std::string v);

  void emit_def_handle(std::string handle, const node &n, std::string field);
  void emit_def_handle(std::string handle, std::string suffix, const node &n,
                       std::string field);

  void emit_def(std::string handle, std::string field);

  void emit_include(std::string d);

  std::string platform_define(node n, std::string suffix);
  std::string platform_define_offset(node n, std::string suffix);

  Inline *create_inline_dec(std::string name, std::string ret_type,
                            std::string arg) const;
  Inline *create_inline_dec(std::string name, std::string ret_type,
                            std::string arg1, std::string arg2) const;
  Inline *create_inline_def(std::string name, std::string ret_type,
                            std::string body, std::string ret_value,
                            std::string arg) const;
  Inline *create_inline_def(std::string name, std::string ret_type,
                            std::string body, std::string ret_value,
                            std::string arg1, std::string arg2) const;
  void add_inline_body(Inline *func, std::string body,
                       std::string ret_value) const;
  void emit_inline_retun_type(bool declare, std::string type);
  void emit_inline_name(std::string type, std::string field);
  void emit_inline_arg(std::ostream &os, std::list<std::string> params);
  void emit_inline_body(Inline::Stage stage, std::string condition,
                        std::string rv);
  void emit_inline_dec(Inline *func, std::string device);
  void emit_inline_def(Inline *func, std::string device);

  void emit_struct_pointer_begin(std::string type, std::string name,
                                 std::string ext);
  void emit_struct_pointer_element(std::string type, uint32_t id,
                                   std::string field, std::string delimiter);
  void emit_struct_pointer_end(std::string empty);

  void emit_struct_decl(std::string type, const node &n);
  void emit_struct_decl(std::string type, std::string suffix, const node &n);

  void emit_struct_begin(std::string type, const node &n);
  void emit_struct_begin(std::string type, std::string suffix, const node &n);

  void emit_struct_field(std::string field, std::string value);
  void emit_struct_field_null(std::string field);
  void emit_struct_field_u32(std::string field, uint32_t value);
  void emit_struct_field_ta(std::string field, target_addr value);
  void emit_struct_field_ts(std::string field, target_size value);
  void emit_struct_field_platform_define(std::string field, node n,
                                         std::string suffix);
  void emit_struct_field_platform_define_offset(std::string field, node n,
                                                std::string suffix);
  void emit_struct_field_platform_define_offset(std::string compat_override,
                                                std::string field, node n,
                                                std::string suffix);

  void emit_struct_container_node_and_array(int size, std::string field1,
                                            const node &c,
                                            std::string subfield1,
                                            std::string field2, uint32_t elem);

  void emit_struct_field_array_elem(int idx, std::string field, uint32_t elem);
  void emit_struct_field_node(std::string field, const node &n,
                              std::string subfield);

  void emit_struct_end(void);

  void emit_struct_array_def_begin(std::string type, std::string name,
                                   std::string size);
  void emit_struct_array_elem_node(const node &n);
};

#endif
