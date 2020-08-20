/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

/* DeviceTree Library */
#include <fdt.h++>
#include <libfdt.h++>

/* Generic Devices */
#include <device.h>
#include <fixed_clock.h>
#include <fixed_factor_clock.h>
#include <memory.h>
#include <stdout_path.h>

/** RISC-V Devices */
#include <riscv_clint0.h>
#include <riscv_cpu.h>
#include <riscv_cpu_intc.h>
#include <riscv_plic0.h>
#include <riscv_pmp.h>

/* SiFive Blocks */
#include <sifive_buserror0.h>
#include <sifive_ccache0.h>
#include <sifive_clic0.h>
#include <sifive_global_external_interrupts0.h>
#include <sifive_gpio0.h>
#include <sifive_gpio_button.h>
#include <sifive_gpio_led.h>
#include <sifive_gpio_switch.h>
#include <sifive_i2c0.h>
#include <sifive_l2pf0.h>
#include <sifive_local_external_interrupts0.h>
#include <sifive_pwm0.h>
#include <sifive_rtc0.h>
#include <sifive_simuart0.h>
#include <sifive_spi0.h>
#include <sifive_test0.h>
#include <sifive_trace.h>
#include <sifive_uart0.h>
#include <sifive_wdog0.h>
#include <ucb_htif0.h>

/* FE310-G000 Devices */
#include <sifive_fe310_g000_hfrosc.h>
#include <sifive_fe310_g000_hfxosc.h>
#include <sifive_fe310_g000_lfrosc.h>
#include <sifive_fe310_g000_pll.h>
#include <sifive_fe310_g000_prci.h>

/* STL */
#include <ctime>
#include <fstream>
#include <iomanip>
#include <iostream>
#include <list>
#include <map>
#include <sstream>
#include <string>

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

static void prepare_devices(const fdt &dtb, fstream &os,
                            std::list<Device *> &devices) {
  /* Generic Devices */
  devices.push_back(new fixed_clock(os, dtb));
  devices.push_back(new fixed_factor_clock(os, dtb));
  devices.push_back(new memory(os, dtb));
  devices.push_back(new stdout_path(os, dtb));

  /* RISC-V Devices */
  devices.push_back(new riscv_clint0(os, dtb));
  devices.push_back(new riscv_cpu(os, dtb));
  devices.push_back(new riscv_cpu_intc(os, dtb));
  devices.push_back(new riscv_plic0(os, dtb));
  devices.push_back(new riscv_pmp(os, dtb));

  /* SiFive Blocks */
  devices.push_back(new sifive_buserror0(os, dtb));
  devices.push_back(new sifive_ccache0(os, dtb));
  devices.push_back(new sifive_clic0(os, dtb));
  devices.push_back(new sifive_local_external_interrupts0(os, dtb));
  devices.push_back(new sifive_global_external_interrupts0(os, dtb));
  devices.push_back(new sifive_gpio0(os, dtb));
  devices.push_back(new sifive_gpio_button(os, dtb));
  devices.push_back(new sifive_gpio_led(os, dtb));
  devices.push_back(new sifive_gpio_switch(os, dtb));
  devices.push_back(new sifive_i2c0(os, dtb));
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

  /* FE310-G000 Devices */
  devices.push_back(new sifive_fe310_g000_hfrosc(os, dtb));
  devices.push_back(new sifive_fe310_g000_hfxosc(os, dtb));
  devices.push_back(new sifive_fe310_g000_lfrosc(os, dtb));
  devices.push_back(new sifive_fe310_g000_pll(os, dtb));
  devices.push_back(new sifive_fe310_g000_prci(os, dtb));
}

static void write_h_file(const fdt &dtb, fstream &os, std::string h_file,
                         std::string release) {
  format_config_file_name(h_file);

  write_banner(os, release);

  os << "#ifndef ASSEMBLY" << std::endl << std::endl;

  os << "#include <metal/machine/platform.h>" << std::endl << std::endl;

  std::list<Device *> devices;

  prepare_devices(dtb, os, devices);

  os << "#ifdef __METAL_MACHINE_MACROS" << std::endl << std::endl;

  os << "#ifndef MACROS_IF_" << h_file << std::endl;
  os << "#define MACROS_IF_" << h_file << std::endl << std::endl;

  for (auto it = devices.begin(); it != devices.end(); it++) {
    (*it)->create_machine_macros();
  }
  os << std::endl;

  os << "#endif /* MACROS_IF_" << h_file << "*/" << std::endl << std::endl;
  os << "#else /* ! __METAL_MACHINE_MACROS */" << std::endl << std::endl;

  os << "#ifndef MACROS_ELSE_" << h_file << std::endl;
  os << "#define MACROS_ELSE_" << h_file << std::endl << std::endl;

  for (auto it = devices.begin(); it != devices.end(); it++) {
    (*it)->create_defines();
  }
  os << std::endl;

  for (auto it = devices.begin(); it != devices.end(); it++) {
    (*it)->include_headers();
  }
  os << std::endl;

  for (auto it = devices.begin(); it != devices.end(); it++) {
    (*it)->declare_structs();
  }
  os << std::endl;

  for (auto it = devices.begin(); it != devices.end(); it++) {
    (*it)->define_inlines();
  }
  os << std::endl;

  for (auto it = devices.begin(); it != devices.end(); it++) {
    (*it)->create_handles();
  }

  os << "#endif /* MACROS_ELSE_" << h_file << "*/" << std::endl << std::endl;
  os << "#endif /* ! __METAL_MACHINE_MACROS */" << std::endl << std::endl;
  os << "#endif /* ! ASSEMBLY */" << std::endl;
}

static void write_i_file(const fdt &dtb, fstream &os, std::string i_file,
                         std::string release) {
  format_config_file_name(i_file);

  write_banner(os, release);

  os << "#ifndef ASSEMBLY" << std::endl << std::endl;

  os << "#ifndef " << i_file << std::endl;
  os << "#define " << i_file << std::endl << std::endl;

  os << "#include <metal/machine.h>" << std::endl << std::endl;

  std::list<Device *> devices;

  prepare_devices(dtb, os, devices);

  for (auto it = devices.begin(); it != devices.end(); it++) {
    (*it)->declare_inlines();
  }
  os << std::endl;

  for (auto it = devices.begin(); it != devices.end(); it++) {
    (*it)->define_structs();
  }
  os << std::endl;

  os << "#endif /* " << i_file << "*/" << std::endl;
  os << "#endif /* ! ASSEMBLY */" << std::endl;
}

int main(int argc, char *argv[]) {
  string dtb_file;
  string header_file;
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
          header_file = argv[++i];
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

  if (!header_file.empty()) {
    string filename = header_file;
    string inline_file =
        filename.insert(header_file.find_last_of("."), "-inline");
    std::fstream h_file;
    std::fstream i_file;

    h_file.open(header_file, fstream::in | fstream::out | fstream::trunc);
    if (h_file.is_open() == false) {
      std::cerr << "Error: Failed to create " << header_file << std::endl;
      return 1;
    }
    i_file.open(inline_file, fstream::in | fstream::out | fstream::trunc);
    if (i_file.is_open() == false) {
      std::cerr << "Error: Failed to create " << inline_file << std::endl;
      return 1;
    }

    write_h_file(f, h_file, header_file, release);
    write_i_file(f, i_file, inline_file, release);
  }

  return 0;
}
