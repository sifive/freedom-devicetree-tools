#include <regs.h>

#include <iostream>

static uint32_t get_address_cells(const node &n) {
  if (n.field_exists("#address-cells")) {
    return n.get_field<uint32_t>("#address-cells");
  } else {
    /* node::num_addr_cells() returns the address cells of the node's parent */
    return n.num_addr_cells();
  }
}

static uint32_t get_size_cells(const node &n) {
  if (n.field_exists("#size-cells")) {
    return n.get_field<uint32_t>("#size-cells");
  } else {
    /* node::num_size_cells() returns the size cells of the node's parent */
    return n.num_size_cells();
  }
}

regs_t get_regs(const node &n) {
  uint32_t addr_cells = n.num_addr_cells();
  uint32_t size_cells = get_size_cells(n);

  if (addr_cells > 2 || size_cells > 2) {
    std::cerr << "Unable to support cells > 2 in node " << n.handle()
              << std::endl;
    exit(1);
  }

  auto cells = n.get_fields<uint32_t>("reg");
  regs_t regs;

  for (int i = 0; i < cells.size(); i += (addr_cells + size_cells)) {
    uint64_t addr = 0;
    uint64_t size = 0;

    if (addr_cells == 1) {
      addr = cells.at(i);
    } else {
      addr = ((uint64_t)cells.at(i) << 32) + (uint64_t)cells.at(i + 1);
    }

    if (size_cells == 1) {
      size = cells.at(i + addr_cells);
    } else {
      size = ((uint64_t)cells.at(i + addr_cells) << 32) +
             (uint64_t)cells.at(i + addr_cells + 1);
    }

    regs.push_back(reg_t(addr, size));
  }

  return regs;
}
