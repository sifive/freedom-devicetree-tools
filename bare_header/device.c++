/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include "bare_header/device.h"

#include <algorithm>
#include <iostream>
#include <set>
#include <string>

Device::Device(std::ostream &os, const fdt &dtb, std::string compat_string)
  : os(os), dtb(dtb), compat_string(compat_string)
{}

void Device::emit_comment(const node &n) {
  os << "/* From " << n.name() << " */\n";
}

string Device::def_handle(const node &n) {
  string name = compat_string;
  string instance = n.instance();

  std::transform(name.begin(), name.end(), name.begin(),
      [](unsigned char c) -> char { 
	if(c == ',' || c == '-') {
	  return '_';
	}
	return toupper(c);
      });
  std::transform(instance.begin(), instance.end(), instance.begin(), toupper);

  return "METAL_" + name + "_" + instance;
}

uint64_t Device::base_address(const node &n) {
  uint64_t b;

  n.named_tuples(
    "reg-names", "reg",
    "control", tuple_t<target_addr, target_size>(), [&](target_addr base, target_size size) {
	b = base;
    });

  return b;
}

void Device::emit_base(const node &n) {
  os << "#define " << def_handle(n) << "_" METAL_BASE_ADDRESS_LABEL << " " << base_address(n) << "UL" << std::endl;
}

uint64_t Device::size(const node &n) {
  uint64_t s;

  n.named_tuples(
    "reg-names", "reg",
    "control", tuple_t<target_addr, target_size>(), [&](target_addr base, target_size size) {
	s = size;
  });

  return s;
}

void Device::emit_size(const node &n) {
  os << "#define " << def_handle(n) << "_" << METAL_SIZE_LABEL << " " << size(n) << "UL" << std::endl;
}

void Device::emit_compat() {
  string compat = compat_string;
  std::transform(compat.begin(), compat.end(), compat.begin(),
      [](unsigned char c) -> char { 
	if(c == ',' || c == '-') {
	  return '_';
	}
	return toupper(c);
      });
  os << "#define METAL_" << compat << std::endl;
}

void Device::emit_offset(string offset_name, uint32_t offset) {
  string name = compat_string;
  std::transform(name.begin(), name.end(), name.begin(),
      [](unsigned char c) -> char { 
	if(c == ',' || c == '-') {
	  return '_';
	}
	return toupper(c);
      });

  os << "#define METAL_" << name << "_" << offset_name << " " << offset << "UL" << std::endl;
}

void Device::emit_property_u32(const node &n, string property_name, uint32_t value) {
  os << "#define " << def_handle(n) << "_" << property_name << " " << value << "UL" << std::endl;
}

