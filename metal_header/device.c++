/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include "metal_header/device.h"

#include <iostream>
#include <set>
#include <string>

Device::Device(std::ostream &os, const fdt &dtb, std::string compat_string)
  : os(os), dtb(dtb), compat_string(compat_string)
{}

void Device::emit_comment(const node &n) {
  os << "/* From " << n.name() << " */\n";
}

void Device::emit_handle(std::string d, const node &n, std::string v) {
  emit_comment(n);
  os << "#define __METAL_DTB_HANDLE_" << d << "_" << n.handle() << " " << v << "\n";
}

void Device::emit_def_handle(std::string handle, const node &n, std::string field) {
  emit_comment(n);
  os << "#define " << handle << " (&__metal_dt_" << n.handle() << field << ")\n\n";
  os << "#define " << "__METAL_DT_" << n.handle_cap() << "_HANDLE (&__metal_dt_"
     << n.handle() << field << ")\n\n";
}

void Device::emit_def(std::string handle, std::string field) {
  os << "#define " << handle << " " << field << "\n\n";
}

void Device::emit_def_value(std::string name, const node &n, std::string field) {
  uint32_t irqs;
  os << "#define __METAL_" << n.handle_cap() << "_INTERRUPTS \t\t";
  if (name.compare("interrupts") == 0) {
    irqs = n.get_fields_count<uint32_t>(name);
  } else {
    irqs = n.get_fields_count<std::tuple<node, uint32_t>>(name);
  }
  os << irqs << "\n";
  if (field.compare("UART_INTERRUPTS") == 0) {
  } else {
    os << "#define METAL_MAX_" << field << "\t __METAL_" << n.handle_cap() << "_INTERRUPTS\n\n";
  }
}

void Device::emit_include(std::string d) {
  static std::set<std::string> included;
  if (included.find(d) != included.end())
    return;
  os << "#include <metal/drivers/" << d << ".h>\n";
  included.insert(d);
}

void Device::emit_struct_pointer_begin(std::string type, std::string name, std::string ext) {
  os << "asm (\".weak " << name << "\");\n";
  os << "struct __metal_driver_" << type << " *" << name << ext << " = {\n";
}
void Device::emit_struct_pointer_element(std::string type, uint32_t id,
				       std::string field, std::string delimiter) {
  os << "\t\t\t\t\t&__metal_dt_" << type << "_" << id << field << delimiter;
}
void Device::emit_struct_pointer_end(std::string empty) {
  os << "\t\t\t\t\t" << empty << " };\n";
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
  os << "asm (\".weak __metal_dt_" << n.handle() << "\");\n";
  os << "struct __metal_driver_" << type << " __metal_dt_" << n.handle() << ";\n\n";
}

void Device::emit_struct_begin(std::string type, const node &n) {
  emit_comment(n);
  os << "struct __metal_driver_" << type << " __metal_dt_" << n.handle() << " = {\n";
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

void Device::emit_struct_container_node_and_array(int size, std::string field1,
						const node& c, std::string subfield1,
						std::string field2, uint32_t elem) {
  static int cna = 0;
  if (cna == 0) {
      os << "    ." << field1 << " = &" << "__metal_dt_" << c.handle() << subfield1 << ",\n";
  }
  os << "    ." << field2 << "[" << cna << "] = " << elem << ",\n";
  cna++;
  if (cna == size) {
      cna = 0;
  }
}

void Device::emit_struct_field_array_elem(int idx, std::string field, uint32_t elem) {
  os << "    ." << field << "[" << idx << "] = " << elem << ",\n";
}

void Device::emit_struct_field_node(std::string field, const node& n, std::string subfield) {
  emit_comment(n);
  os << "    ." << field << " = &" << "__metal_dt_" << n.handle() << subfield << ",\n";
}

void Device::emit_struct_end(void) {
  os << "};\n\n";
}

void Device::emit_struct_array_def_begin(std::string type, std::string name, std::string size) {
  os << "/* Custom array definition */\n";
  os << "asm (\".weak __metal_dt_" << name << "\");\n";
  os << "struct __metal_driver_" << type << " __metal_dt_" << name << "[ ] = {\n";
}

void Device::emit_struct_array_elem_node(const node& n) {
  os << "                    &__metal_dt_" << n.handle() << ",\n";
}

