/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

/* DeviceTree Library */
#include "fdt.h++"
#include "libfdt.h++"

/* Generic Devices */
#include "metal_header/device.h"
#include "metal_header/fixed_clock.h"

/* RISC-V Devices */
#include "metal_header/riscv_clint0.h"
#include "metal_header/riscv_cpu.h"
#include "metal_header/riscv_cpu_intc.h"
#include "metal_header/riscv_plic0.h"
#include "metal_header/riscv_pmp.h"

/* SiFive Blocks */
#include "metal_header/sifive_clic0.h"
#include "metal_header/sifive_local_external_interrupts0.h"
#include "metal_header/sifive_global_external_interrupts0.h"
#include "metal_header/sifive_gpio0.h"
#include "metal_header/sifive_gpio_button.h"
#include "metal_header/sifive_gpio_led.h"
#include "metal_header/sifive_gpio_switch.h"
#include "metal_header/sifive_test0.h"
#include "metal_header/sifive_uart0.h"

/* FE310-G000 Devices */
#include "metal_header/sifive_fe310_g000_hfrosc.h"
#include "metal_header/sifive_fe310_g000_hfxosc.h"
#include "metal_header/sifive_fe310_g000_pll.h"
#include "metal_header/sifive_fe310_g000_prci.h"

/* STL */
#include <fstream>
#include <iostream>
#include <map>
#include <list>
#include <string>

using std::cerr;
using std::endl;
using std::fstream;
using std::string;

static void show_usage(string name)
{
  std::cerr << "Usage: " << name << " <option(s)>\n"
	    << "Options:\n"
	    << "\t-h,--help\t\t\tShow this help message\n"
	    << "\t-d,--dtb <eg. xxx.dtb>\t\tSpecify fullpath to the DTB file\n"
	    << "\t-o,--output <eg. ${machine}.h>\tGenerate machine header file\n"
	    << endl;
}

void search_replace_all(std::string& str, const std::string& from, const std::string& to) {
    if (from.empty() || (from.compare(to) == 0))
        return;
    size_t pos = 0;
    while((pos = str.find(from, pos)) != std::string::npos) {
        str.replace(pos, from.length(), to);
        pos += to.length();
    }
}

static void write_config_file(const fdt &dtb, fstream &os, std::string cfg_file)
{
  std::transform(cfg_file.begin(), cfg_file.end(), cfg_file.begin(),
                   [](unsigned char c) { return (c == '-') ? '_' : toupper(c); });
  std::transform(cfg_file.begin(), cfg_file.end(), cfg_file.begin(),
                   [](unsigned char c) { return (c == '.') ? '_' : c; });

  search_replace_all(cfg_file, "/", "__");

  os << "#ifndef ASSEMBLY" << std::endl << std::endl;

  os << "#ifndef " << cfg_file << std::endl;
  os << "#define " << cfg_file << std::endl << std::endl;

  std::list<Device *> devices;

  /* Generic Devices */
  devices.push_back(new fixed_clock(os, dtb));

  /* RISC-V Devices */
  devices.push_back(new riscv_clint0(os, dtb));
  devices.push_back(new riscv_cpu(os, dtb));
  devices.push_back(new riscv_cpu_intc(os, dtb));
  devices.push_back(new riscv_plic0(os, dtb));
  devices.push_back(new riscv_pmp(os, dtb));

  /* SiFive Blocks */
  devices.push_back(new sifive_clic0(os, dtb));
  devices.push_back(new sifive_local_external_interrupts0(os, dtb));
  devices.push_back(new sifive_global_external_interrupts0(os, dtb));
  devices.push_back(new sifive_gpio0(os, dtb));
  devices.push_back(new sifive_gpio_button(os, dtb));
  devices.push_back(new sifive_gpio_led(os, dtb));
  devices.push_back(new sifive_gpio_switch(os, dtb));
  devices.push_back(new sifive_test0(os, dtb));
  devices.push_back(new sifive_uart0(os, dtb));

  /* FE310-G000 Devices */
  devices.push_back(new sifive_fe310_g000_hfrosc(os, dtb));
  devices.push_back(new sifive_fe310_g000_hfxosc(os, dtb));
  devices.push_back(new sifive_fe310_g000_pll(os, dtb));
  devices.push_back(new sifive_fe310_g000_prci(os, dtb));

  os << "#ifdef __METAL_MACHINE_MACROS" << std::endl << std::endl;

  for(auto it = devices.begin(); it != devices.end(); it++) {
    (*it)->create_machine_macros();
  }
  os << std::endl << "#else /* ! __METAL_MACHINE_MACROS */" << std::endl << std::endl;

  for(auto it = devices.begin(); it != devices.end(); it++) {
    (*it)->create_defines();
  }
  os << std::endl;

  for(auto it = devices.begin(); it != devices.end(); it++) {
    (*it)->include_headers();
  }
  os << std::endl;

  for(auto it = devices.begin(); it != devices.end(); it++) {
    (*it)->declare_structs();
  }
  os << std::endl;

  for(auto it = devices.begin(); it != devices.end(); it++) {
    (*it)->define_structs();
  }
  os << std::endl;

  for(auto it = devices.begin(); it != devices.end(); it++) {
    (*it)->create_handles();
  }
  os << std::endl;

  os << "#endif /* ! __METAL_MACHINE_MACROS */" << std::endl;
  os << "#endif /* " << cfg_file << "*/" << std::endl;
  os << "#endif /* ! ASSEMBLY */" << std::endl;
}

int main (int argc, char* argv[])
{
  string dtb_file;
  string config_file;

  if ((argc < 2) && (argc > 5)) {
      show_usage(argv[0]);
      return 1;
  } else {
      for (int i = 1; i < argc; ++i) {
          string arg = argv[i];
          if ((arg == "-d") || (arg == "--dtb")) {
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

  fdt f(dtb_file);

  if (!config_file.empty()) {
    std::fstream cfg;

    cfg.open(config_file, fstream::in | fstream::out | fstream::trunc);
    if (cfg.is_open() == false) {
      std::cerr << "Error: Failed to create " << config_file << std::endl;
      return 1;
    }

    write_config_file(f, cfg, config_file);
  }

  return 0;
}
