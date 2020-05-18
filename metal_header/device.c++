/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include "metal_header/device.h"

#include <iostream>
#include <set>
#include <string>

Device::Device(std::ostream &os, const fdt &dtb, std::string compat_string)
    : os(os), dtb(dtb), compat_string(compat_string) {}

void Device::emit_comment(const node &n) {
  os << "/* From " << n.name() << " */\n";
}

void Device::emit_handle(std::string d, const node &n, std::string v) {
  emit_comment(n);
  os << "#define __METAL_DTB_HANDLE_" << d << "_" << n.handle() << " " << v
     << "\n";
}

void Device::emit_def_handle(std::string handle, const node &n,
                             std::string field) {
  emit_comment(n);
  os << "#define " << handle << " (&__metal_dt_" << n.handle() << field
     << ")\n\n";
  os << "#define "
     << "__METAL_DT_" << n.handle_cap() << "_HANDLE (&__metal_dt_" << n.handle()
     << field << ")\n\n";
}

void Device::emit_def_handle(std::string handle, std::string suffix,
                             const node &n, std::string field) {
  emit_comment(n);
  os << "#define " << handle << " (&__metal_dt_" << n.handle() << "_" << suffix
     << field << ")\n\n";
}

void Device::emit_def(std::string handle, std::string field) {
  os << "#define " << handle << " " << field << "\n\n";
}

void Device::emit_include(std::string d) {

  auto to_include = [](std::string input_string) -> std::string {
    std::string s = input_string;
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) -> char {
      if (c == ',') {
        return '_';
      }
      return c;
    });
    return s;
  };

  static std::set<std::string> included;
  if (included.find(d) != included.end())
    return;

  os << "#include <metal/drivers/" << to_include(d) << ".h>\n";
  included.insert(d);
}

Inline *Device::create_inline_dec(std::string name, std::string ret_type,
                                  std::string arg) const {
  Inline *func;
  func = new Inline(name, ret_type);
  func->args.push_back(arg);

  return func;
}

Inline *Device::create_inline_dec(std::string name, std::string ret_type,
                                  std::string arg1, std::string arg2) const {
  Inline *func = create_inline_dec(name, ret_type, arg1);
  func->args.push_back(arg2);
  return func;
}

Inline *Device::create_inline_def(std::string name, std::string ret_type,
                                  std::string body, std::string ret_value,
                                  std::string arg) const {
  Inline *func;
  func = new Inline(name, ret_type);
  func->args.push_back(arg);
  func->body_cases.push_back(body);
  func->body_returns.push_back(ret_value);

  return func;
}

Inline *Device::create_inline_def(std::string name, std::string ret_type,
                                  std::string body, std::string ret_value,
                                  std::string arg1, std::string arg2) const {
  Inline *func = create_inline_def(name, ret_type, body, ret_value, arg1);
  func->args.push_back(arg2);
  return func;
}

void Device::add_inline_body(Inline *func, std::string body,
                             std::string ret_value) const {
  func->body_cases.push_back(body);
  func->body_returns.push_back(ret_value);
}

void Device::emit_inline_retun_type(bool declare, std::string type) {
  (declare == true) ? (os << "extern __inline__ " << type << " ")
                    : (os << "static __inline__ " << type << " ");
}
void Device::emit_inline_name(std::string type, std::string field) {
  os << "__metal_driver_" << type << "_" << field << "(";
}

void Device::emit_inline_body(Inline::Stage stage, std::string condition,
                              std::string return_value) {
  switch (stage) {
  case Inline::Start:
    os << "{\n";
    os << "\tif (" << condition << ") {\n";
    os << "\t\treturn " << return_value << ";\n";
    os << "\t}\n";
    break;
  case Inline::Middle:
    os << "\telse if (" << condition << ") {\n";
    os << "\t\treturn " << return_value << ";\n";
    os << "\t}\n";
    break;
  case Inline::End:
    os << "\telse {\n";
    os << "\t\treturn " << return_value << ";\n";
    os << "\t}\n";
    os << "}\n\n";
    break;
  case Inline::Empty:
  defaut:
    os << "{\n";
    os << "\t\treturn " << return_value << ";\n";
    os << "}\n\n";
    break;
  }
}

void Device::emit_inline_arg(std::ostream &os, std::list<std::string> params) {
  while (!params.empty()) {
    os << params.front();
    if (params.size() > 1) {
      os << ", ";
    }
    params.pop_front();
  }
}

void Device::emit_inline_dec(Inline *func, std::string device) {
  emit_inline_retun_type(true, func->return_type);
  emit_inline_name(device, func->name);
  emit_inline_arg(os, func->args);
  os << ");\n";
}

void Device::emit_inline_def(Inline *func, std::string device) {
  emit_inline_retun_type(false, func->return_type);
  emit_inline_name(device, func->name);
  emit_inline_arg(os, func->args);
  os << ")\n";
  bool start = true;
  std::string bc, br;
  Inline::Stage stage;
  while (!func->body_cases.empty()) {
    bc = func->body_cases.front();
    func->body_cases.pop_front();
    br = func->body_returns.front();
    func->body_returns.pop_front();
    if (bc == "empty") {
      stage = Inline::Empty;
    } else if (bc == "else") {
      stage = Inline::End;
    } else {
      if (start == true) {
        stage = Inline::Start;
        start = false;
      } else {
        stage = Inline::Middle;
      }
    }
    emit_inline_body(stage, bc, br);
  }
}

void Device::emit_struct_pointer_begin(std::string type, std::string name,
                                       std::string ext) {
  os << "__asm__ (\".weak " << name << "\");\n";
  os << "struct __metal_driver_" << type << " *" << name << ext << " = {\n";
}
void Device::emit_struct_pointer_element(std::string type, uint32_t id,
                                         std::string field,
                                         std::string delimiter) {
  os << "\t\t\t\t\t&__metal_dt_" << type << "_" << id << field << delimiter;
}
void Device::emit_struct_pointer_end(std::string empty) {
  os << "\t\t\t\t\t" << empty << " };\n";
}

std::string Device::platform_define(node n, std::string suffix) {
  auto to_define = [](std::string input_string) -> std::string {
    std::string s = input_string;
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) -> char {
      if (c == ',' || c == '-') {
        return '_';
      }
      return toupper(c);
    });

    return s;
  };
  std::string name = to_define(n.get_fields<std::string>("compatible")[0]);
  std::string instance = to_define(n.instance());
  suffix = to_define(suffix);

  return "METAL_" + name + "_" + instance + "_" + suffix;
}

std::string Device::platform_define_offset(node n, std::string suffix) {
  auto to_define = [](std::string input_string) -> std::string {
    std::string s = input_string;
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) -> char {
      if (c == ',' || c == '-') {
        return '_';
      }
      return toupper(c);
    });

    return s;
  };
  std::string name = to_define(n.get_fields<std::string>("compatible")[0]);
  suffix = to_define(suffix);

  return "METAL_" + name + "_" + suffix;
}

/* Emits the header for a structure.  This is particularly tricky: here we're
 * telling GCC that the structure is a constant, but then telling the
 * assembler than it can be optimized.  This allows GCC to inline these
 * constants when that helps code generation, but still allows the linker to
 * merge together multiple copies of the structure so we don't use too much
 * memory.  This isn't technically kosher, but since we own the entire
 * "__metal_" namespace */
void Device::emit_struct_decl(std::string type, const node &n) {
  emit_comment(n);
  os << "extern struct __metal_driver_" << type << " __metal_dt_" << n.handle()
     << ";\n\n";
}

void Device::emit_struct_decl(std::string type, std::string suffix,
                              const node &n) {
  emit_comment(n);
  os << "extern struct __metal_driver_" << type << "_" << suffix
     << " __metal_dt_" << n.handle() << "_" << suffix << ";\n\n";
}

void Device::emit_struct_begin(std::string type, const node &n) {
  emit_comment(n);
  os << "struct __metal_driver_" << type << " __metal_dt_" << n.handle()
     << " = {\n";
}

void Device::emit_struct_begin(std::string type, std::string suffix,
                               const node &n) {
  emit_comment(n);
  os << "struct __metal_driver_" << type << "_" << suffix << " __metal_dt_"
     << n.handle() << "_" << suffix << " = {\n";
}

void Device::emit_struct_field(std::string field, std::string value) {
  os << "    ." << field << " = " << value << ",\n";
}

void Device::emit_struct_field_null(std::string field) {
  os << "    ." << field << " = NULL,\n";
}

void Device::emit_struct_field_u32(std::string field, uint32_t value) {
  os << "    ." << field << " = " << std::to_string(value) << "UL,\n";
}

void Device::emit_struct_field_ta(std::string field, target_addr value) {
  os << "    ." << field << " = " << std::to_string(value) << "UL,\n";
}

void Device::emit_struct_field_ts(std::string field, target_size value) {
  os << "    ." << field << " = " << std::to_string(value) << "UL,\n";
}

void Device::emit_struct_field_platform_define(std::string field, node n,
                                               std::string suffix) {
  auto to_define = [](std::string input_string) -> std::string {
    std::string s = input_string;
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) -> char {
      if (c == ',' || c == '-') {
        return '_';
      }
      return toupper(c);
    });

    return s;
  };
  std::string name = to_define(compat_string);
  std::string instance = to_define(n.instance());
  suffix = to_define(suffix);

  os << "    ." << field << " = METAL_" << name << "_" << instance << "_"
     << suffix << "," << std::endl;
}

void Device::emit_struct_field_platform_define_offset(std::string field, node n,
                                                      std::string suffix) {
  emit_struct_field_platform_define_offset(compat_string, field, n, suffix);
}

void Device::emit_struct_field_platform_define_offset(
    std::string compat_override, std::string field, node n,
    std::string suffix) {
  auto to_define = [](std::string input_string) -> std::string {
    std::string s = input_string;
    std::transform(s.begin(), s.end(), s.begin(), [](unsigned char c) -> char {
      if (c == ',' || c == '-') {
        return '_';
      }
      return toupper(c);
    });

    return s;
  };
  std::string name = to_define(compat_override);
  suffix = to_define(suffix);

  os << "    ." << field << " = METAL_" << name << "_" << suffix << ","
     << std::endl;
}

void Device::emit_struct_container_node_and_array(int size, std::string field1,
                                                  const node &c,
                                                  std::string subfield1,
                                                  std::string field2,
                                                  uint32_t elem) {
  static int cna = 0;
  if (cna == 0) {
    os << "    ." << field1 << " = &"
       << "__metal_dt_" << c.parent().handle() + "_" + c.handle() << subfield1
       << ",\n";
  }
  os << "    ." << field2 << "[" << cna << "] = " << elem << ",\n";
  cna++;
  if (cna == size) {
    cna = 0;
  }
}

void Device::emit_struct_field_array_elem(int idx, std::string field,
                                          uint32_t elem) {
  os << "    ." << field << "[" << idx << "] = " << elem << ",\n";
}

void Device::emit_struct_field_node(std::string field, const node &n,
                                    std::string subfield) {
  emit_comment(n);
  os << "    ." << field << " = &"
     << "__metal_dt_" << n.handle() << subfield << ",\n";
}

void Device::emit_struct_end(void) { os << "};\n\n"; }

void Device::emit_struct_array_def_begin(std::string type, std::string name,
                                         std::string size) {
  os << "/* Custom array definition */\n";
  os << "__asm__ (\".weak __metal_dt_" << name << "\");\n";
  os << "struct __metal_driver_" << type << " __metal_dt_" << name
     << "[ ] = {\n";
}

void Device::emit_struct_array_elem_node(const node &n) {
  os << "                    &__metal_dt_" << n.handle() << ",\n";
}
