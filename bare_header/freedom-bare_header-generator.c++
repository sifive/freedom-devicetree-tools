/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

/* DeviceTree Library */
#include "fdt.h++"
#include "libfdt.h++"

/* STL */
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <string>

/* Generic Devices */
#include "bare_header/device.h"
#include "bare_header/fixed_clock.h"
#include "bare_header/fixed_factor_clock.h"

/* RISC-V Devices */
#include "bare_header/riscv_clint0.h"
#include "bare_header/riscv_plic0.h"

/* SiFive Blocks */
#include "bare_header/sifive_aon0.h"
#include "bare_header/sifive_buserror0.h"
#include "bare_header/sifive_ccache0.h"
#include "bare_header/sifive_clic0.h"
#include "bare_header/sifive_error0.h"
#include "bare_header/sifive_fe310_g000_hfrosc.h"
#include "bare_header/sifive_fe310_g000_hfxosc.h"
#include "bare_header/sifive_fe310_g000_lfrosc.h"
#include "bare_header/sifive_fe310_g000_pll.h"
#include "bare_header/sifive_fe310_g000_prci.h"
#include "bare_header/sifive_global_external_interrupts0.h"
#include "bare_header/sifive_gpio0.h"
#include "bare_header/sifive_gpio_buttons.h"
#include "bare_header/sifive_gpio_leds.h"
#include "bare_header/sifive_gpio_switches.h"
#include "bare_header/sifive_hca_0_5_x.h"
#include "bare_header/sifive_i2c0.h"
#include "bare_header/sifive_l2pf0.h"
#include "bare_header/sifive_local_external_interrupts0.h"
#include "bare_header/sifive_pwm0.h"
#include "bare_header/sifive_rtc0.h"
#include "bare_header/sifive_simuart0.h"
#include "bare_header/sifive_spi0.h"
#include "bare_header/sifive_test0.h"
#include "bare_header/sifive_trace.h"
#include "bare_header/sifive_uart0.h"
#include "bare_header/sifive_wdog0.h"
#include "bare_header/ucb_htif0.h"

using std::cerr;
using std::endl;
using std::fstream;
using std::string;

static void write_banner(fstream &os, std::string rel_tag) {
  os << "/* Copyright 2019 SiFive, Inc */" << std::endl;
  os << "/* SPDX-License-Identifier: Apache-2.0 */" << std::endl;
  os << "/* ----------------------------------- */" << std::endl;
  if (!rel_tag.empty()) {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    os << "/* [" << rel_tag << "] " << std::put_time(&tm, "%d-%m-%Y %H-%M-%S")
       << "        */" << std::endl;
  }
  os << "/* ----------------------------------- */" << std::endl << std::endl;
}

static void show_usage(string name) {
  std::cerr
      << "Usage: " << name << " <option(s)>\n"
      << "Options:\n"
      << "\t-h,--help\t\t\tShow this help message\n"
      << "\t-d,--dtb <eg. xxx.dtb>\t\tSpecify fullpath to the DTB file\n"
      << "\t-o,--output <eg. ${machine}.h>\tGenerate machine header file\n"
      << endl;
}

/* Splits a string into a list of strings on character c */
std::list<std::string> split_string(std::string &str, char c) {
  std::list<std::string> parts;

  std::istringstream stream(str);

  std::string substr;
  while (std::getline(stream, substr, c)) {
    parts.push_back(substr);
  }

  return parts;
}

void search_replace_all(std::string &str, const std::string &from,
                        const std::string &to) {
  if (from.empty() || (from.compare(to) == 0))
    return;
  size_t pos = 0;
  while ((pos = str.find(from, pos)) != std::string::npos) {
    str.replace(pos, from.length(), to);
    pos += to.length();
  }
}

void uppercase_string(std::string &str) {
  std::transform(str.begin(), str.end(), str.begin(),
                 [](unsigned char c) -> char { return toupper(c); });
}

void format_config_file_name(std::string &cfg_file) {
  /* Isolate the name of the file and its containing folder */
  auto parts = split_string(cfg_file, '/');
  while (parts.size() > 2) {
    parts.pop_front();
  }

  /* Format the file path into an #define */
  search_replace_all(cfg_file, "-", "_");
  search_replace_all(cfg_file, ".", "_");
  search_replace_all(cfg_file, "/", "__");
  search_replace_all(cfg_file, ":", "__");

  uppercase_string(cfg_file);
}

static void write_config_file(const fdt &dtb, fstream &os, std::string cfg_file,
                              std::string release) {
  format_config_file_name(cfg_file);

  write_banner(os, release);

  os << "#ifndef " << cfg_file << std::endl;
  os << "#define " << cfg_file << std::endl << std::endl;

  std::list<Device *> devices;

  /* Generic Devices */
  devices.push_back(new fixed_clock(os, dtb));
  devices.push_back(new fixed_factor_clock(os, dtb));

  /* RISC-V Devices */
  devices.push_back(new riscv_clint0(os, dtb));
  devices.push_back(new riscv_plic0(os, dtb));

  /* SiFive Blocks */
  devices.push_back(new sifive_aon0(os, dtb));
  devices.push_back(new sifive_buserror0(os, dtb));
  devices.push_back(new sifive_ccache0(os, dtb));
  devices.push_back(new sifive_clic0(os, dtb));
  devices.push_back(new sifive_error0(os, dtb));
  devices.push_back(new sifive_fe310_g000_hfrosc(os, dtb));
  devices.push_back(new sifive_fe310_g000_hfxosc(os, dtb));
  devices.push_back(new sifive_fe310_g000_lfrosc(os, dtb));
  devices.push_back(new sifive_fe310_g000_prci(os, dtb));
  devices.push_back(new sifive_fe310_g000_pll(os, dtb));
  devices.push_back(new sifive_global_external_interrupts0(os, dtb));
  devices.push_back(new sifive_gpio0(os, dtb));
  devices.push_back(new sifive_gpio_buttons(os, dtb));
  devices.push_back(new sifive_gpio_leds(os, dtb));
  devices.push_back(new sifive_gpio_switches(os, dtb));
  devices.push_back(new sifive_i2c0(os, dtb));
  devices.push_back(new sifive_local_external_interrupts0(os, dtb));
  devices.push_back(new sifive_l2pf0(os, dtb));
  devices.push_back(new sifive_pwm0(os, dtb));
  devices.push_back(new sifive_rtc0(os, dtb));
  devices.push_back(new sifive_spi0(os, dtb));
  devices.push_back(new sifive_test0(os, dtb));
  devices.push_back(new sifive_trace(os, dtb));
  devices.push_back(new sifive_uart0(os, dtb));
  devices.push_back(new sifive_simuart0(os, dtb));
  devices.push_back(new sifive_wdog0(os, dtb));
  devices.push_back(new ucb_htif0(os, dtb));
  devices.push_back(new sifive_hca_0_5_x(os, dtb));

  for (auto it = devices.begin(); it != devices.end(); it++) {
    (*it)->emit_defines();
    (*it)->emit_offsets();
  }

  os << "#endif /* " << cfg_file << "*/" << std::endl;
}

int main(int argc, char *argv[]) {
  string dtb_file;
  string config_file;
  string release;

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
      } else if (arg == "-r") {
        if (i + 1 < argc) {
          release = argv[++i];
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

    write_config_file(f, cfg, config_file, release);
  }

  return 0;
}
