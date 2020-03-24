/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include "bare_header/device.h"

#include <algorithm>
#include <iostream>
#include <ranges.h>
#include <set>
#include <string>

Device::Device(std::ostream &os, const fdt &dtb, std::string compat_string)
    : os(os), dtb(dtb), compat_string(compat_string) {}

int Device::get_index(const node &n) { return get_index(n, compat_string); }

int Device::get_index(const node &n, string compat) {
  /* Compare nodes by base address */
  struct node_compare {
    bool operator()(const node &n1, const node &n2) const {
      return (n1.instance().compare(n2.instance()) < 0);
    }
  };

  /* Build a list of all matching nodes, sorted by base address */
  std::set<node, node_compare> matching_nodes;

  dtb.match(std::regex(compat),
            [&](const node &n) { matching_nodes.insert(n); });

  /* Return the index of node n in the list */
  int count = 0;
  for (auto it = matching_nodes.begin(); it != matching_nodes.end(); it++) {
    if (((*it).instance().compare(n.instance())) == 0) {
      return count;
    }
    count++;
  }
  return -1;
}

void Device::emit_comment(const node &n) {
  os << "/* From " << n.name() << " */\n";
}

string Device::def_handle(const node &n) {
  string name = n.get_fields<string>("compatible")[0];

  return def_handle(name, n);
}

string Device::def_handle(std::string name, const node &n) {
  string instance = n.instance();

  std::transform(name.begin(), name.end(), name.begin(),
                 [](unsigned char c) -> char {
                   if (c == ',' || c == '-') {
                     return '_';
                   }
                   return toupper(c);
                 });

  std::transform(instance.begin(), instance.end(), instance.begin(), toupper);

  return "METAL_" + name + "_" + instance;
}

string Device::def_handle_index(const node &n) {
  string name = n.get_fields<string>("compatible")[0];
  return def_handle_index(name, n);
}

string Device::def_handle_index(std::string name, const node &n) {
  string instance = std::to_string(get_index(n));

  std::transform(name.begin(), name.end(), name.begin(),
                 [](unsigned char c) -> char {
                   if (c == ',' || c == '-') {
                     return '_';
                   }
                   return toupper(c);
                 });
  std::transform(instance.begin(), instance.end(), instance.begin(), toupper);

  return "METAL_" + name + "_" + instance;
}

typedef struct {
  uint64_t base;
  uint64_t size;
} mem_map_t;

static mem_map_t extract_mem_map(const node &n) {
  mem_map_t m;

  if (n.field_exists("reg-names")) {
    n.named_tuples("reg-names", "reg",
                   /* "control" goes first, if this exists then it will return
                    * the control registers and not the "mem" registers */
                   "control", tuple_t<target_addr, target_size>(),
                   [&](target_addr b, target_size s) {
                     m.base = b;
                     m.size = s;
                   },
                   /* Return the "mem" registers if and only if there is no
                    * "control" registers */
                   "mem", tuple_t<target_addr, target_size>(),
                   [&](target_addr b, target_size s) {
                     m.base = b;
                     m.size = s;
                   });
  } else if (n.field_exists("ranges")) {
    ranges_t ranges = get_ranges(n);

    /* TODO: How do we pick which of the ranges entries to use? */
    if (!ranges.empty()) {
      m.base = ranges.front().child_address;
      m.size = ranges.front().size;
    }
  } else {
    auto regs = n.get_fields<std::tuple<target_addr, target_size>>("reg");

    if (!regs.empty()) {
      m.base = std::get<0>(regs.front());
      m.size = std::get<1>(regs.front());
    }
  }

  return m;
}

uint64_t Device::base_address(const node &n) { return extract_mem_map(n).base; }

void Device::emit_base(const node &n) {
  os << "#define " << def_handle(n) << "_" METAL_BASE_ADDRESS_LABEL << " "
     << base_address(n) << "UL" << std::endl;

  // If the address is very small, it already is an index.
  if (n.instance().length() > 2) {
    os << "#define " << def_handle_index(n) << "_" METAL_BASE_ADDRESS_LABEL
       << " " << base_address(n) << "UL" << std::endl;
  }
}

void Device::emit_base(std::string compat, const node &n) {
  os << "#define " << def_handle(compat, n) << "_" METAL_BASE_ADDRESS_LABEL
     << " " << base_address(n) << "UL" << std::endl;

  // If the address is very small, it already is an index.
  if (n.instance().length() > 2) {
    os << "#define " << def_handle_index(compat, n)
       << "_" METAL_BASE_ADDRESS_LABEL << " " << base_address(n) << "UL"
       << std::endl;
  }
}

uint64_t Device::size(const node &n) { return extract_mem_map(n).size; }

void Device::emit_size(const node &n) {
  os << "#define " << def_handle(n) << "_" << METAL_SIZE_LABEL << " " << size(n)
     << "UL" << std::endl;

  // If the address is very small, it already is an index.
  if (n.instance().length() > 2) {
    os << "#define " << def_handle_index(n) << "_" << METAL_SIZE_LABEL << " "
       << size(n) << "UL" << std::endl;
  }
}

void Device::emit_size(std::string compat, const node &n) {
  os << "#define " << def_handle(compat, n) << "_" << METAL_SIZE_LABEL << " "
     << size(n) << "UL" << std::endl;

  // If the address is very small, it already is an index.
  if (n.instance().length() > 2) {
    os << "#define " << def_handle_index(compat, n) << "_" << METAL_SIZE_LABEL
       << " " << size(n) << "UL" << std::endl;
  }
}

void Device::emit_compat() { emit_compat(compat_string); }

void Device::emit_compat(string compat) {
  std::transform(compat.begin(), compat.end(), compat.begin(),
                 [](unsigned char c) -> char {
                   if (c == ',' || c == '-' || c == '.') {
                     return '_';
                   }
                   return toupper(c);
                 });
  os << "#define METAL_" << compat << std::endl;
}

void Device::emit_offset(string name, string offset_name, uint32_t offset) {
  std::transform(name.begin(), name.end(), name.begin(),
                 [](unsigned char c) -> char {
                   if (c == ',' || c == '-' || c == '.') {
                     return '_';
                   }
                   return toupper(c);
                 });

  os << "#define METAL_" << name << "_" << offset_name << " " << offset << "UL"
     << std::endl;
}

void Device::emit_offset(string offset_name, uint32_t offset) {
  emit_offset(compat_string, offset_name, offset);
}

void Device::emit_property_u32(const node &n, string property_name,
                               uint32_t value) {
  os << "#define " << def_handle(n) << "_" << property_name << " " << value
     << "UL" << std::endl;
  // If the address is very small, it already is an index.
  if (n.instance().length() > 2) {
    os << "#define " << def_handle_index(n) << "_" << property_name << " "
       << value << "UL" << std::endl;
  }
}
