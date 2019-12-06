/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <ctime>
#include <fcntl.h>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <iterator>
#include <sstream>
#include <stdarg.h>
#include <stdlib.h>
#include <string>
#include <unistd.h>
#include <vector>
#ifdef __cplusplus
extern "C" {
#endif
#include <libfdt.h>
#ifdef __cplusplus
}
#endif
#include "multilib.h++"
#include <fdt.h++>

using std::cout;
using std::endl;
using std::fstream;
using std::ifstream;
using std::ofstream;
using std::string;

static void write_banner(fstream &os, std::string rel_tag) {
  os << "# Copyright 2019 SiFive, Inc #" << std::endl;
  os << "# SPDX-License-Identifier: Apache-2.0 #" << std::endl;
  os << "# ----------------------------------- #" << std::endl;
  if (!rel_tag.empty()) {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    os << "# [" << rel_tag << "] " << std::put_time(&tm, "%d-%m-%Y %H-%M-%S")
       << "        #" << std::endl;
  }
  os << "# ----------------------------------- #" << std::endl << std::endl;
}

static char *dts_blob;

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

static string get_dts_attribute(const string path, const string tag) {
  int node, len;
  const char *value;

  node = fdt_path_offset(dts_blob, path.c_str());
  if (node < 0)
    return "";
  value = (const char *)fdt_getprop(dts_blob, node, tag.c_str(), &len);
  if (!value)
    return "";
  std::cout << string(value) << std::endl;
  return string(value);
}

static uint32_t memory_port_width() {
  if (!dts_blob)
    return 0;

  uint32_t port_width = 32;

  auto dtb = fdt((const uint8_t *)dts_blob);

  dtb.chosen("metal,entry", tuple_t<node, uint32_t, uint32_t>(),
             [&](node n, uint32_t index, uint32_t offset) {
               if (n.parent().field_exists("sifive,port-width-bytes")) {
                 port_width = 8 * n.parent().get_field<uint32_t>(
                                      "sifive,port-width-bytes");
                 std::cout << "Port-width of " << port_width
                           << " bits found for " << n.parent().name()
                           << std::endl;
               } else {
                 std::cout << "No port-width found for " << n.parent().name()
                           << ", default to 32 bits!" << std::endl;
               }
             });

  return port_width;
}

static void show_dts_attributes(void) {
  std::cout << "#" << __FUNCTION__ << std::endl;
  std::cout << std::endl << std::endl;
  get_dts_attribute("/cpus/cpu@0", "riscv,isa");
  get_dts_attribute("/soc/serial@20000000", "compatible");
}

static void write_config_file(fstream &os, std::string board,
                              std::string release) {
  string isa;
  string cpucompat;
  string mmutype;
  string serial;
  string boothart_str;

  write_banner(os, release);

  // Get the CPU compatible string
  auto dtb = fdt((const uint8_t *)dts_blob);
  int boot_hart = 0;
  dtb.chosen("metal,boothart", tuple_t<node>(),
             [&](node n) { boot_hart = std::stoi(n.instance()); });
  boothart_str = "/cpus/cpu@" + std::to_string(boot_hart);

  isa = arch2arch(get_dts_attribute(boothart_str, "riscv,isa"));
  std::size_t found = isa.find("rv64");

  os << "RISCV_ARCH=" << isa << std::endl;
  os << "RISCV_ABI=" << arch2abi(isa) << std::endl;
  if (found != std::string::npos) {
    os << "RISCV_CMODEL=medany" << std::endl;
  } else {
    os << "RISCV_CMODEL=medlow" << std::endl;
  }

  cpucompat = get_dts_attribute(boothart_str, "compatible");
  // Append SIFIVE series
  if (cpucompat.find("bullet") != std::string::npos) {
    os << "RISCV_SERIES=sifive-7-series" << std::endl;
  } else if (cpucompat.find("caboose") != std::string::npos) {
    os << "RISCV_SERIES=sifive-2-series" << std::endl;
  } else if (cpucompat.find("rocket") != std::string::npos) {
    if (found != std::string::npos) {
      os << "RISCV_SERIES=sifive-5-series" << std::endl;
    } else {
      os << "RISCV_SERIES=sifive-3-series" << std::endl;
    }
  }
  os << std::endl;

  // Get the mmu-type property
  mmutype = get_dts_attribute("/cpus/cpu@0", "mmu-type");
  if (mmutype == "") {
    // If CPU 0 doesn't have mmu-type, make sure that CPU 1 (if it exists)
    // also doesn't. This is needed for U54-MC, where CPU 0 is the S51 hart.
    mmutype = get_dts_attribute("/cpus/cpu@1", "mmu-type");
  }

  if (board.find("rtl") != std::string::npos) {
    os << "COREIP_MEM_WIDTH=" << std::to_string(memory_port_width())
       << std::endl;
    os << std::endl;
    os << "TARGET_TAGS=rtl" << std::endl;
    os << "TARGET_DHRY_ITERS=2000" << std::endl;
    os << "TARGET_CORE_ITERS=5" << std::endl;
  } else if (board.find("qemu") != std::string::npos) {
    os << "TARGET_TAGS=qemu" << std::endl;
    os << "TARGET_DHRY_ITERS=20000000" << std::endl;
    os << "TARGET_CORE_ITERS=5000" << std::endl;
  } else if (board.find("spike") != std::string::npos) {
    os << "TARGET_TAGS=spike" << std::endl;
    os << "TARGET_DHRY_ITERS=20000000" << std::endl;
    os << "TARGET_CORE_ITERS=5000" << std::endl;
  } else if (board.find("arty") != std::string::npos) {
    os << "TARGET_TAGS=fpga openocd" << std::endl;
    os << "TARGET_DHRY_ITERS=20000000" << std::endl;
    os << "TARGET_CORE_ITERS=5000" << std::endl;
  } else if (board.find("vc707") != std::string::npos) {
    os << "TARGET_TAGS=fpga openocd" << std::endl;
    os << "TARGET_DHRY_ITERS=20000000" << std::endl;
    os << "TARGET_CORE_ITERS=5000" << std::endl;
  } else if (board.find("hifive1-revb") != std::string::npos) {
    os << "TARGET_TAGS=board jlink" << std::endl;
    os << "TARGET_DHRY_ITERS=20000000" << std::endl;
    os << "TARGET_CORE_ITERS=5000" << std::endl;
  } else {
    os << "TARGET_TAGS=board openocd" << std::endl;
    os << "TARGET_DHRY_ITERS=20000000" << std::endl;
    os << "TARGET_CORE_ITERS=5000" << std::endl;
  }
}

static void show_usage(string name) {
  std::cerr
      << "Usage: " << name << " <option(s)>\n"
      << "Options:\n"
      << "\t-h,--help\t\t\tShow this help message\n"
      << "\t-b,--board <eg. rtl | arty | hifive>\t\tSpecify board type\n"
      << "\t-d,--dtb <eg. xxx.dtb>\t\tSpecify fullpath to the DTB file\n"
      << "\t-o,--output <eg. openocd.cfg>\t\tGenerate openocd config file\n"
      << "\t-s,--show \t\tShow openocd config file on std-output\n"
      << endl;
}

int main(int argc, char *argv[]) {
  bool show = false;
  string board = "arty";
  ;
  string dtb_file;
  string config_file;
  string release;

  if ((argc < 2) && (argc > 5)) {
    show_usage(argv[0]);
    return 1;
  } else {
    for (int i = 1; i < argc; ++i) {
      string arg = argv[i];
      if ((arg == "-b") || (arg == "--board")) {
        if (i + 1 < argc) {
          board = argv[++i];
          if ((board.find("rtl") != std::string::npos) ||
              (board.find("arty") != std::string::npos) ||
              (board.find("vc707") != std::string::npos) ||
              (board.find("spike") != std::string::npos) ||
              (board.find("qemu") != std::string::npos) ||
              (board.find("hifive") != std::string::npos)) {
            std::cout << "Board type given is " << board << std::endl;
          } else {
            std::cerr << "Possible options are <rtl | arty | hifive>."
                      << std::endl;
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
      } else if (arg == "-r") {
        if (i + 1 < argc) {
          release = argv[++i];
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

  if (!config_file.empty()) {
    std::fstream cfg;

    cfg.open(config_file, fstream::in | fstream::out | fstream::trunc);
    if (cfg.is_open() == false) {
      std::cerr << "Error: Failed to create " << config_file << std::endl;
      return 1;
    }

    write_config_file(cfg, board, release);
  }

  if (show) {
    show_dts_attributes();
  }

  return 0;
}
