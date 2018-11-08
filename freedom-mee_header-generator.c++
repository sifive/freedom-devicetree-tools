/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include "fdt.h++"
#include "libfdt.h++"
#include <fstream>
#include <iostream>
#include <map>
#include <set>
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

static void write_config_file(const fdt &dtb, fstream &os)
{
  int cpus;
  os << "#ifndef ASSEMBLY\n";

  auto emit_comment = [&](const node &n) {
    os << "/* From " << n.name() << " */\n";
  };

  auto emit_handle = [&](std::string d, const node &n, std::string v) {
    emit_comment(n);
    os << "#define __MEE_DTB_HANDLE_" << d << "_" << n.handle() << " " << v << "\n";
  };

  auto emit_def_handle = [&](std::string handle, const node &n, std::string field) {
    emit_comment(n);
    os << "#define " << handle << " (&__mee_dt_" << n.handle() << field << ")\n";
  };

  auto emit_def_index_handle = [&](std::string handle, std::string name, std::string field) {
    os << "#define " << handle << "(idx)" << " (__mee_dt_" << name << "[idx]" << field << ")\n\n";
  };

  auto emit_def_key_handle = [&](std::string handle, std::string key, std::string name, std::string field) {
    os << "#define " << handle << "(" << key << ")" << " (&__mee_dt_" << name << "_#" << key << field << ")\n\n";
  };

  auto emit_def = [&](std::string handle, std::string field) {
    os << "#define " << handle << " " << field << "\n\n";
  };

  auto emit_def_instance = [&](std::string handle, const node &n, std::string field) {
    os << "#define " << handle << "_" << n.handle()
       << " " << ((field != "") ? field : n.instance()) << "\n\n";
  };

  std::set<std::string> included;
  auto emit_include = [&](std::string d) {
    if (included.find(d) != included.end())
      return;
    os << "#include <mee/drivers/" << d << ".h>\n";
    included.insert(d);
  };

  /* Emits the header for a structure.  This is particularly tricky: here we're
   * telling GCC that the structure is a constant, but then telling the
   * assembler than it can be optimized.  This allows GCC to inline these
   * constants when that helps code generation, but still allows the linker to
   * merge together multiple copies of the structure so we don't use too much
   * memory.  This isn't technically kosher, but since we own the entire
   * "__mee_" namespace */
  auto emit_struct_decl = [&](std::string type, const node &n) {
    emit_comment(n);
    os << "asm (\".weak __mee_dt_" << n.handle() << "\");\n";
    os << "struct __mee_driver_" << type << " __mee_dt_" << n.handle() << ";\n\n";
  };

  auto emit_struct_begin = [&](std::string type, const node &n) {
    emit_comment(n);
    os << "struct __mee_driver_" << type << " __mee_dt_" << n.handle() << " = {\n";
  };

  auto emit_struct_field = [&](std::string field, std::string value) {
    os << "    ." << field << " = " << value << ",\n";
  };

  auto emit_struct_field_null = [&](std::string field) {
    os << "    ." << field << " = NULL,\n";
  };

  auto emit_struct_field_u32 = [&](std::string field, uint32_t value) {
    os << "    ." << field << " = " << std::to_string(value) << "UL,\n";
  };

  auto emit_struct_field_ta = [&](std::string field, target_addr value) {
    os << "    ." << field << " = " << std::to_string(value) << "UL,\n";
  };

  auto emit_struct_field_ts = [&](std::string field, target_size value) {
    os << "    ." << field << " = " << std::to_string(value) << "UL,\n";
  };

  auto emit_struct_field_node = [&](std::string field, const node& n, std::string subfield) {
    emit_comment(n);
    os << "    ." << field << " = &" << "__mee_dt_" << n.handle() << subfield << ",\n";
  };

  auto emit_struct_end = [&](void) {
    os << "};\n\n";
  };

  auto emit_struct_array_def_begin = [&](std::string type, std::string name, std::string size) {
    os << "/* Custom array definition */\n";
    os << "asm (\".weak __mee_dt_" << name << "\");\n";
    os << "struct __mee_driver_" << type << " __mee_dt_" << name << "[ ] = {\n";
  };

  auto emit_struct_array_elem_node = [&](const node& n) {
    os << "                    &__mee_dt_" << n.handle() << ",\n";
  };

  /* First, find the required headers that must be included in order to make
   * this a sane MEE instance. */
  dtb.match(
    std::regex("cpu"),                      [&](node n) { emit_include("riscv,cpu");                },
    std::regex("fixed-clock"),              [&](node n) { emit_include("fixed-clock");              },
    std::regex("sifive,fe310-g000,pll"),    [&](node n) { emit_include("sifive,fe310-g000,pll");    },
    std::regex("sifive,fe310-g000,prci"),   [&](node n) { emit_include("sifive,fe310-g000,prci");   },
    std::regex("sifive,fe310-g000,hfxosc"), [&](node n) { emit_include("sifive,fe310-g000,hfxosc"); },
    std::regex("sifive,fe310-g000,hfrosc"), [&](node n) { emit_include("sifive,fe310-g000,hfrosc"); },
    std::regex("sifive,gpio0"),             [&](node n) { emit_include("sifive,gpio0");             },
    std::regex("sifive,uart0"),             [&](node n) { emit_include("sifive,uart0");             },
    std::regex("sifive,test0"),             [&](node n) { emit_include("sifive,test0");             }
  );

  /* Now emit the various nodes's header definitons. */
  dtb.match(
    std::regex("cpu"),                      [&](node n) { emit_struct_decl("cpu",                      n); },
    std::regex("fixed-clock"),              [&](node n) { emit_struct_decl("fixed_clock",              n); },
    std::regex("sifive,fe310-g000,pll"),    [&](node n) { emit_struct_decl("sifive_fe310_g000_pll",    n); },
    std::regex("sifive,fe310-g000,prci"),   [&](node n) { emit_struct_decl("sifive_fe310_g000_prci",   n); },
    std::regex("sifive,fe310-g000,hfxosc"), [&](node n) { emit_struct_decl("sifive_fe310_g000_hfxosc", n); },
    std::regex("sifive,fe310-g000,hfrosc"), [&](node n) { emit_struct_decl("sifive_fe310_g000_hfrosc", n); },
    std::regex("sifive,gpio0"),             [&](node n) { emit_struct_decl("sifive_gpio0",             n); },
    std::regex("sifive,uart0"),             [&](node n) { emit_struct_decl("sifive_uart0",             n); },
    std::regex("sifive,test0"),             [&](node n) { emit_struct_decl("sifive_test0",             n); }
  );

  /* Walk through the device tree, emitting various nodes as we know about
   * them. */
  cpus = 0;
  dtb.match(
    std::regex("cpu"), [&](node n) {
      emit_struct_begin("cpu", n);
      emit_struct_field("timer_vtable", "&__mee_driver_vtable_cpu");
      emit_struct_field("timer.vtable", "&__mee_driver_vtable_cpu.timer_vtable");
      emit_struct_field_u32("timebase", n.get_field<uint32_t>("timebase-frequency"));
      emit_struct_end();
      emit_def_instance("__MEE_DT_TIMEBASE_FREQUENCY", n, std::to_string(n.get_field<uint32_t>("timebase-frequency")));
      emit_def_instance("__MEE_DT_HART", n, "");
      cpus++;
    }, std::regex("fixed-clock"), [&](node n) {
      emit_struct_begin("fixed_clock", n);
      emit_struct_field("vtable", "&__mee_driver_vtable_fixed_clock");
      emit_struct_field("clock.vtable", "&__mee_driver_vtable_fixed_clock.clock");
      emit_struct_field_u32("rate", n.get_field<uint32_t>("clock-frequency"));
      emit_struct_end();
    }, std::regex("sifive,fe310-g000,pll"), [&](node n) {
      emit_struct_begin("sifive_fe310_g000_pll", n);
      emit_struct_field("vtable", "&__mee_driver_vtable_sifive_fe310_g000_pll");
      emit_struct_field("clock.vtable", "&__mee_driver_vtable_sifive_fe310_g000_pll.clock");
      n.named_tuples(
        "clock-names", "clocks",
        "pllref", tuple_t<node>(), [&](node base) {
          emit_struct_field_node("pllref", base, ".clock");
        },
        "pllsel0", tuple_t<node>(), [&](node base) {
          emit_struct_field_node("pllsel0", base, ".clock");
        });
      n.named_tuples(
        "reg-names", "reg",
        "config", tuple_t<node, target_size>(), [&](node base, target_size off) {
          emit_struct_field_node("config_base", base, "");
          emit_struct_field_ts("config_offset", off);
        },
        "divider", tuple_t<node, target_size>(), [&](node base, target_size off) {
          emit_struct_field_node("divider_base", base, "");
          emit_struct_field_ts("divider_offset", off);
        });
      emit_struct_field_u32("init_rate", n.get_field<uint32_t>("clock-frequency"));
      emit_struct_end();
      emit_def_handle("__MEE_DT_SIFIVE_FE310_G000_PLL_HANDLE", n, "");
    }, std::regex("sifive,fe310-g000,prci"), [&](node n) {
      emit_struct_begin("sifive_fe310_g000_prci", n);
      emit_struct_field("vtable", "&__mee_driver_vtable_sifive_fe310_g000_prci");
      n.named_tuples(
        "reg-names", "reg",
        "mem", tuple_t<target_addr, target_size>(), [&](target_addr base, target_size size) {
          emit_struct_field_ta("base", base);
          emit_struct_field_ts("size", size);
        });
      emit_struct_end();
    }, std::regex("sifive,fe310-g000,hfxosc"), [&](node n) {
      emit_struct_begin("sifive_fe310_g000_hfxosc", n);
      emit_struct_field("vtable", "&__mee_driver_vtable_sifive_fe310_g000_hfxosc");
      emit_struct_field("clock.vtable", "&__mee_driver_vtable_sifive_fe310_g000_hfxosc.clock");
      emit_struct_field_node("ref", n.get_field<node>("clocks"), ".clock");
      n.named_tuples(
        "reg-names", "reg",
        "config", tuple_t<node, target_size>(), [&](node base, target_size offset) {
          emit_struct_field_node("config_base", base, "");
          emit_struct_field_ts("config_offset", offset);
        });
      emit_struct_end();
    }, std::regex("sifive,fe310-g000,hfrosc"), [&](node n) {
      emit_struct_begin("sifive_fe310_g000_hfrosc", n);
      emit_struct_field("vtable", "&__mee_driver_vtable_sifive_fe310_g000_hfrosc");
      emit_struct_field("clock.vtable", "&__mee_driver_vtable_sifive_fe310_g000_hfrosc.clock");
      emit_struct_field_node("ref", n.get_field<node>("clocks"), ".clock");
      n.named_tuples(
        "reg-names", "reg",
        "config", tuple_t<node, target_size>(), [&](node base, target_size offset) {
          emit_struct_field_node("config_base", base, "");
          emit_struct_field_ts("config_offset", offset);
        });
      emit_struct_end();
    }, std::regex("sifive,gpio0"), [&](node n) {
      emit_struct_begin("sifive_gpio0", n);
      emit_struct_field("vtable", "&__mee_driver_vtable_sifive_gpio0");
      n.named_tuples(
        "reg-names", "reg",
        "control", tuple_t<target_addr, target_size>(), [&](target_addr base, target_size size) {
          emit_struct_field_ta("base", base);
          emit_struct_field_ts("size", size);
        });
      emit_struct_end();
    }, std::regex("sifive,uart0"), [&](node n) {
      emit_struct_begin("sifive_uart0", n);
      emit_struct_field("vtable", "&__mee_driver_vtable_sifive_uart0");
      emit_struct_field("uart.vtable", "&__mee_driver_vtable_sifive_uart0.uart");
      n.named_tuples(
        "reg-names", "reg",
        "control", tuple_t<target_addr, target_size>(), [&](target_addr base, target_size size) {
          emit_struct_field_ta("control_base", base);
          emit_struct_field_ts("control_size", size);
        });
      n.maybe_tuple(
        "clocks", tuple_t<node>(),
        [&](){ emit_struct_field_null("clock"); },
        [&](node n) { emit_struct_field_node("clock", n, ".clock"); });
      n.maybe_tuple(
        "pinmux", tuple_t<node, uint32_t, uint32_t>(),
        [&](){ emit_struct_field_null("pinmux"); },
        [&](node n, uint32_t dest, uint32_t source) {
            emit_struct_field_node("pinmux", n, "");
            emit_struct_field_u32("pinmux_output_selector", dest);
            emit_struct_field_u32("pinmux_source_selector", source);
        });
      emit_struct_end();
    }, std::regex("sifive,test0"), [&](node n) {
      emit_struct_begin("sifive_test0", n);
      emit_struct_field("vtable", "&__mee_driver_vtable_sifive_test0");
      emit_struct_field("shutdown.vtable", "&__mee_driver_vtable_sifive_test0.shutdown");
      n.named_tuples(
        "reg-names", "reg",
        "control", tuple_t<target_addr, target_size>(), [&](target_addr base, target_size size) {
          emit_struct_field_ta("base", base);
          emit_struct_field_ts("size", size);
        });
      emit_struct_end();
      emit_def_handle("__MEE_DT_SHUTDOWN_HANDLE", n, ".shutdown");
    }
  );

  /* Create links from the internal MEE environment into the user-visible
   * environment. */
  dtb.chosen(
    "stdout-path", tuple_t<std::string>(), [&](std::string info) {
      if (info.find(":") == decltype(info)::npos)
        return;
      auto path = info.substr(0, info.find(":"));
      auto target = dtb.node_by_path(path);
      emit_def_handle("__MEE_DT_STDOUT_UART_HANDLE", target, ".uart");
      auto baud = info.substr(info.find(":")+1);
      emit_def("__MEE_DT_STDOUT_UART_BAUD", baud);
    }
  );

  /* Create a list of cpus timers. */
  emit_def("__MEE_DT_MAX_HARTS", std::to_string(cpus));
  emit_def_key_handle("__MEE_DT_TIMER_HANDLE", "key", "cpu", ".timer");
  emit_struct_array_def_begin("cpu *", "cpus", "__MEE_DT_MAX_HARTS");
  dtb.match(
    std::regex("cpu"), [&](node n) { emit_struct_array_elem_node(n); }
  );
  emit_struct_end();
  emit_def_index_handle("__MEE_DT_CPU_HANDLE", "cpus", "");

  os << "#endif/*ASSEMBLY*/\n";
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

    write_config_file(f, cfg);
  }

  return 0;
}
