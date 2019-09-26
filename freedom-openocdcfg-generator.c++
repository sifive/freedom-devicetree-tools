/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <fcntl.h>
#include <fstream>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdarg.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <vector>

#include <fdt.h++>

using std::cout;
using std::endl;
using std::fstream;
using std::ifstream;
using std::ofstream;
using std::string;
class memory {
public:
  std::string mem_type;
  std::string mem_alias;
  std::string mem_name;
  uint64_t mem_start;
  uint64_t mem_length;

  memory() {}
  memory(std::string, std::string, std::string, uint64_t, uint64_t);
};

memory::memory(std::string mtype, std::string alias, std::string name,
               uint64_t start, uint64_t length) {
  mem_type = mtype;
  mem_alias = alias;
  mem_name = name;
  mem_start = start;
  mem_length = length;
}

static char *dts_blob;
static std::vector<memory> dts_memory_list;

static bool has_memory(std::string mem) {
  for (auto entry : dts_memory_list) {
    if (entry.mem_alias.find(mem) == std::string::npos) {
      std::cout << "found " << mem << std::endl;
      return true;
    }
  }
  return false;
}

static void alias_memory(std::string from, std::string to) {
  std::vector<memory>::iterator it;

  for (it = dts_memory_list.begin(); it != dts_memory_list.end(); ++it) {
    if (it->mem_alias.find(from) != std::string::npos) {
      it->mem_alias = to;
      std::cout << "alias " << from << " to " << it->mem_alias << std::endl;
    }
  }
}

/* Helper functions for extracting the number of address cells and size
 * cells for a node */

static uint64_t get_address_cells(const node &n) {
  if (n.field_exists("#address-cells")) {
    return n.get_field<uint32_t>("#address-cells");
  } else {
    /* The node::num_addr_cells() method gets the address cells of the parent
     * node */
    return n.num_addr_cells();
  }
}

static uint64_t get_size_cells(const node &n) {
  if (n.field_exists("#size-cells")) {
    return n.get_field<uint32_t>("#size-cells");
  } else {
    /* The node::num_size_cells() method gets the address cells of the parent
     * node */
    return n.num_size_cells();
  }
}

/* Helper functions for getting the child address and size from a ranges
 * property */

static target_addr get_memory_ranges_address(const node &n) {
  int child_addr_cells = get_address_cells(n);

  auto cells = n.get_fields<uint32_t>("ranges");

  if (child_addr_cells == 1) {
    return cells.at(0);
  } else if (child_addr_cells == 2) {
    return ((target_addr)cells.at(0) << 32) + (target_addr)cells.at(1);
  } else {
    std::cerr << "Node " << n.handle()
              << " has unsupported number of address cells "
              << std::to_string(child_addr_cells) << std::endl;
  }

  return 0;
}

static target_size get_memory_ranges_size(const node &n) {
  int child_addr_cells = get_address_cells(n);
  int parent_addr_cells = get_address_cells(n.parent());
  int child_size_cells = get_size_cells(n);

  auto cells = n.get_fields<uint32_t>("ranges");

  if (child_size_cells == 1) {
    return cells.at(child_addr_cells + parent_addr_cells);
  } else if (child_size_cells == 2) {
    return ((target_addr)cells.at(child_addr_cells + parent_addr_cells) << 32) +
           (target_addr)cells.at(child_addr_cells + parent_addr_cells + 1);
  } else {
    std::cerr << "Node " << n.handle()
              << " has unsupported number of size cells "
              << std::to_string(child_size_cells) << std::endl;
  }

  return cells.back();
}

template <typename Out>
void split(const std::string &s, char delim, Out result) {
  std::stringstream ss(s);
  std::string item;
  while (std::getline(ss, item, delim)) {
    *(result++) = item;
  }
}

std::vector<std::string> split(const std::string &s, char delim) {
  std::vector<std::string> elems;
  split(s, delim, std::back_inserter(elems));
  return elems;
}

std::string trim(const std::string &str,
                 const std::string &whitespace = " \t") {
  const auto strBegin = str.find_first_not_of(whitespace);
  if (strBegin == std::string::npos)
    return "";

  const auto strEnd = str.find_last_not_of(whitespace);
  const auto strRange = strEnd - strBegin + 1;

  return str.substr(strBegin, strRange);
}

static void get_dts_attribute(const string path, const string tag) {
  int node, len;
  const char *value;

  node = fdt_path_offset(dts_blob, path.c_str());
  value = (const char *)fdt_getprop(dts_blob, node, tag.c_str(), &len);
  std::cout << string(value) << std::endl;
}

static void dts_memory(void) {
  int dtim_count = 0;
  int itim_count = 0;
  int sram_count = 0;
  int testram_count = 0;
  int spi_mem_count = 0;
  int spi_control_count = 0;
  int memory_count = 0;
  int ahb_periph_count = 0;
  int apb_periph_count = 0;
  int axi4_periph_count = 0;
  int tl_periph_count = 0;
  int ahb_sys_count = 0;
  int apb_sys_count = 0;
  int axi4_sys_count = 0;
  int tl_sys_count = 0;

  auto dtb = fdt((const uint8_t *)dts_blob);
  dtb.match(
      std::regex("memory"),
      [&](node n) {
        auto name = n.name();
        n.maybe_tuple("reg", tuple_t<target_addr, target_size>(), [&]() {},
                      [&](target_addr base, target_size size) {
                        if (memory_count == 0)
                          dts_memory_list.push_back(
                              memory("mem", "memory", "memory", base, size));
                        memory_count++;
                      });
      },
      std::regex("sifive,dtim0"),
      [&](node n) {
        auto name = n.name();
        n.named_tuples("reg-names", "reg", "mem",
                       tuple_t<target_addr, target_size>(),
                       [&](target_addr base, target_size size) {
                         if (dtim_count == 0)
                           dts_memory_list.push_back(memory(
                               "mem", "dtim", "sifive,dtim0", base, size));
                         dtim_count++;
                       });
      },
      std::regex("sifive,itim0"),
      [&](node n) {
        auto name = n.name();
        n.named_tuples("reg-names", "reg", "mem",
                       tuple_t<target_addr, target_size>(),
                       [&](target_addr base, target_size size) {
                         if (itim_count == 0)
                           dts_memory_list.push_back(memory(
                               "mem", "itim", "sifive,itim0", base, size));
                         itim_count++;
                       });
      },
      std::regex("sifive,testram0"),
      [&](node n) {
        auto name = n.name();
        n.named_tuples(
            "reg-names", "reg", "mem", tuple_t<target_addr, target_size>(),
            [&](target_addr base, target_size size) {
              if (testram_count == 0)
                dts_memory_list.push_back(
                    memory("mem", "testram", "sifive,testram0", base, size));
              testram_count++;
            });
      },
      std::regex("sifive,ahb-periph-port"),
      [&](node n) {
        if (n.field_exists("ranges")) {
          target_addr address = get_memory_ranges_address(n);
          target_size size = get_memory_ranges_size(n);
          if (ahb_periph_count == 0)
            dts_memory_list.push_back(memory("mem", "ahb_periph_ram",
                                             "sifive,ahb-periph-port", address,
                                             size));

          ahb_periph_count++;
        }
      },
      std::regex("sifive,apb-periph-port"),
      [&](node n) {
        if (n.field_exists("ranges")) {
          target_addr address = get_memory_ranges_address(n);
          target_size size = get_memory_ranges_size(n);
          if (apb_periph_count == 0)
            dts_memory_list.push_back(memory("mem", "apb_periph_ram",
                                             "sifive,apb-periph-port", address,
                                             size));

          apb_periph_count++;
        }
      },
      std::regex("sifive,axi4-periph-port"),
      [&](node n) {
        if (n.field_exists("ranges")) {
          target_addr address = get_memory_ranges_address(n);
          target_size size = get_memory_ranges_size(n);
          if (axi4_periph_count == 0)
            dts_memory_list.push_back(memory("mem", "axi4_periph_ram",
                                             "sifive,axi4-periph-port", address,
                                             size));

          axi4_periph_count++;
        }
      },
      std::regex("sifive,tl-periph-port"),
      [&](node n) {
        if (n.field_exists("ranges")) {
          target_addr address = get_memory_ranges_address(n);
          target_size size = get_memory_ranges_size(n);
          if (tl_periph_count == 0)
            dts_memory_list.push_back(memory("mem", "tl_periph_ram",
                                             "sifive,tl-periph-port", address,
                                             size));

          tl_periph_count++;
        }
      },
      std::regex("sifive,ahb-sys-port"),
      [&](node n) {
        if (n.field_exists("ranges")) {
          target_addr address = get_memory_ranges_address(n);
          target_size size = get_memory_ranges_size(n);
          if (ahb_sys_count == 0)
            dts_memory_list.push_back(memory(
                "mem", "ahb_sys_ram", "sifive,ahb-sys-port", address, size));

          ahb_sys_count++;
        }
      },
      std::regex("sifive,apb-sys-port"),
      [&](node n) {
        if (n.field_exists("ranges")) {
          target_addr address = get_memory_ranges_address(n);
          target_size size = get_memory_ranges_size(n);
          if (apb_sys_count == 0)
            dts_memory_list.push_back(memory(
                "mem", "apb_sys_ram", "sifive,apb-sys-port", address, size));

          apb_sys_count++;
        }
      },
      std::regex("sifive,axi4-sys-port"),
      [&](node n) {
        if (n.field_exists("ranges")) {
          target_addr address = get_memory_ranges_address(n);
          target_size size = get_memory_ranges_size(n);
          if (axi4_sys_count == 0)
            dts_memory_list.push_back(memory(
                "mem", "axi4_sys_ram", "sifive,axi4-sys-port", address, size));

          axi4_sys_count++;
        }
      },
      std::regex("sifive,tl-sys-port"),
      [&](node n) {
        if (n.field_exists("ranges")) {
          target_addr address = get_memory_ranges_address(n);
          target_size size = get_memory_ranges_size(n);
          if (tl_sys_count == 0)
            dts_memory_list.push_back(memory(
                "mem", "tl_sys_ram", "sifive,tl-sys-port", address, size));

          tl_sys_count++;
        }
      },
      std::regex("sifive,sram0"),
      [&](node n) {
        auto name = n.name();
        n.named_tuples("reg-names", "reg", "mem",
                       tuple_t<target_addr, target_size>(),
                       [&](target_addr base, target_size size) {
                         dts_memory_list.push_back(
                             memory("mem", "sram" + std::to_string(sram_count),
                                    "sifive,sram0", base, size));
                         sram_count++;
                       });
      },
      std::regex("sifive,spi0"),
      [&](node n) {
        auto name = n.name();
        n.named_tuples("reg-names", "reg", "control",
                       tuple_t<target_addr, target_size>(),
                       [&](target_addr base, target_size size) {
                         if (spi_control_count == 0)
                           dts_memory_list.push_back(memory(
                               "control", "spi", "sifive,spi0", base, size));
                         spi_control_count++;
                       },
                       "mem", tuple_t<target_addr, target_size>(),
                       [&](target_addr base, target_size size) {
                         if (spi_mem_count == 0)
                           dts_memory_list.push_back(
                               memory("mem", "spi", "sifive,spi0", base, size));
                         spi_mem_count++;
                       });
      });

  if (memory_count > 0) {
    alias_memory("memory", "ram");
  } else if (sram_count > 0) {
    alias_memory("sram", "ram");
  } else if (dtim_count > 0) {
    alias_memory("dtim", "ram");
  } else {
    std::cerr << "Unable to find a suitable memory for RAM" << std::endl;
    exit(1);
  }

  alias_memory("spi", "flash");
}

static void show_dts_memory(void) {
  std::cout << "#" << __FUNCTION__ << std::endl;
  std::cout << std::endl << std::endl;
  std::cout << "MEMORY" << std::endl << "{" << std::endl;
  for (auto entry : dts_memory_list) {
    std::cout << "\t" << entry.mem_alias << "/" << entry.mem_name;
    std::cout << ((entry.mem_type.compare("control") == 0) ? " (rw)"
                                                           : " (rwx)");
    std::cout << " : ORIGIN = 0x" << std::hex << entry.mem_start;
    std::cout << ", LENGTH = 0x" << entry.mem_length;
    std::cout << std::dec << std::endl;
  }
  std::cout << "}" << std::endl;
}

static void write_config_file(fstream &os, std::string board) {
  os << "#" << __FUNCTION__ << std::endl;
  os << "# JTAG adapter setup" << std::endl;
  os << "adapter_khz     10000" << std::endl << std::endl;

  os << "interface ftdi" << std::endl;
  if (board.find("hifive") != string::npos) {
    os << "ftdi_device_desc \"Dual RS232-HS\"" << std::endl;
    os << "ftdi_vid_pid 0x0403 0x6010" << std::endl << std::endl;

    os << "ftdi_layout_init 0x0008 0x001b" << std::endl;
    os << "ftdi_layout_signal nSRST -oe 0x0020" << std::endl;
    os << "ftdi_layout_signal LED -data 0x0020" << std::endl << std::endl;
  } else {
    /* Arty Board by Default */
    os << "ftdi_device_desc \"Olimex OpenOCD JTAG ARM-USB-TINY-H\""
       << std::endl;
    os << "ftdi_vid_pid 0x15ba 0x002a" << std::endl << std::endl;

    os << "ftdi_layout_init 0x0808 0x0a1b" << std::endl;
    os << "ftdi_layout_signal nSRST -oe 0x0200" << std::endl;
    os << "ftdi_layout_signal LED -data 0x0800" << std::endl << std::endl;
  }
  os << "set _CHIPNAME riscv" << std::endl;
  os << "jtag newtap $_CHIPNAME cpu -irlen 5" << std::endl << std::endl;

  os << "set _TARGETNAME $_CHIPNAME.cpu" << std::endl;
  os << "target create $_TARGETNAME.0 riscv -chain-position $_TARGETNAME"
     << std::endl;

  /* Define work area in RAM */
  if (has_memory("ram")) {
    for (auto entry : dts_memory_list) {
      if (entry.mem_alias.compare("ram") == 0) {
        os << "$_TARGETNAME.0 configure "
           << "-work-area-phys 0x" << std::hex << entry.mem_start << std::dec
           << " -work-area-size "
           << (entry.mem_length < 10000 ? entry.mem_length : 10000)
           << " -work-area-backup 1" << std::endl
           << std::endl;
        break;
      }
    }
  } else {
    os << "$_TARGETNAME.0 configure -work-area-phys 0x0"
       << " -work-area-size 10000 -work-area-backup 1" << std::endl
       << std::endl;
  }

  /* Define the SPI flash */
  if (has_memory("flash")) {
    bool mem_found = false;
    bool cntrl_found = false;
    memory spi_mem;
    memory spi_cntrl;

    for (auto entry : dts_memory_list) {
      if (entry.mem_alias.compare("flash") == 0) {
        if (entry.mem_type.compare("mem") == 0 && !mem_found) {
          spi_mem = entry;
          mem_found = true;
        } else if (entry.mem_type.compare("control") == 0 && !cntrl_found) {
          spi_cntrl = entry;
          cntrl_found = true;
        }
      }
    }
    os << "flash bank spi0 fespi 0x" << std::hex
       << (mem_found ? spi_mem.mem_start : 0);
    os << " 0 0 0 $_TARGETNAME.0 0x" << (cntrl_found ? spi_cntrl.mem_start : 0)
       << std::dec << std::endl;
  }

  os << "init" << std::endl;
  os << "if {[ info exists pulse_srst]} {" << std::endl;
  os << "\tftdi_set_signal nSRST 0" << std::endl;
  os << "\tftdi_set_signal nSRST z" << std::endl;
  os << "sleep 1500" << std::endl;
  os << "}" << std::endl;
  os << "halt" << std::endl << std::endl;

  os << "flash protect 0 64 last off" << std::endl;
  os << "echo \"Ready for Remote Connections\"" << std::endl;
}

static void show_usage(string name) {
  std::cerr
      << "Usage: " << name << " <option(s)>\n"
      << "Options:\n"
      << "\t-h,--help\t\t\tShow this help message\n"
      << "\t-p,--protocol <jtag | cjtag>\tSpecify protocol, defaults to jtag\n"
      << "\t-b,--board <eg. arty | hifive1>\tSpecify board type\n"
      << "\t-d,--dtb <eg. xxx.dtb>\t\tSpecify fullpath to the DTB file\n"
      << "\t-o,--output <eg. openocd.cfg>\tGenerate openocd config file\n"
      << "\t-s,--show \t\t\tShow openocd config file on std-output\n"
      << endl;
}

int main(int argc, char *argv[]) {
  bool show = false;
  string board = "arty";
  string protocol = "jtag";
  string dtb_file;
  string config_file;

  if ((argc < 2) && (argc > 5)) {
    show_usage(argv[0]);
    return 1;
  } else {
    for (int i = 1; i < argc; ++i) {
      string arg = argv[i];
      if ((arg == "-b") || (arg == "--board")) {
        if (i + 1 < argc) {
          board = argv[++i];
          if ((board.find("arty") != string::npos) &&
              (board.find("hifive") != string::npos)) {
            std::cerr << "Possible options are <arty | hifive>." << std::endl;
            return 1;
          }
        }
      } else if ((arg == "-d") || (arg == "--dtb")) {
        if (i + 1 < argc) {
          dtb_file = argv[++i];
        } else {
          std::cerr << "--dtb option requires file path." << std::endl;
          show_usage(argv[0]);
          return 1;
        }
      } else if ((arg == "-o") || (arg == "--output")) {
        if (i + 1 < argc) {
          config_file = argv[++i];
        } else {
          std::cerr << "--output option requires file path." << std::endl;
          show_usage(argv[0]);
          return 1;
        }
      } else if ((arg == "-p" || (arg == "--protocol"))) {
        if (i + 1 < argc) {
          protocol = argv[++i];
          if ((protocol != "jtag") && (protocol != "cjtag")) {
            std::cerr << "Possible options for --protocol are <jtag | cjtag>." << std::endl;
            show_usage(argv[0]);
            return 1;
          }
        } else {
          std::cerr << "--protocol option requires an argument <jtag | cjtag." << std::endl;
          show_usage(argv[0]);
          return 1;
        }
      } else if ((arg == "-s") || (arg == "--show")) {
        show = true;
      } else {
        show_usage(argv[0]);
        return 1;
      }
    }
  }

  if (dtb_file.empty()) {
    std::cerr << "--dtb option requires file path." << std::endl;
    show_usage(argv[0]);
    return 1;
  }
  dts_blob = dts_read(dtb_file.c_str());
  if (!dts_blob) {
    std::cerr << "ABORT: Failed to read " << dtb_file << std::endl;
    return 1;
  }

  dts_memory();

  if (!config_file.empty()) {
    std::fstream cfg;

    cfg.open(config_file, fstream::in | fstream::out | fstream::trunc);
    if (cfg.is_open() == false) {
      std::cerr << "Error: Failed to create " << config_file << std::endl;
      return 1;
    }

    write_config_file(cfg, board);
  }

  if (show) {
    show_dts_memory();
  }

  return 0;
}
