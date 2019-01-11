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
  int cpus;
  int leds;
  int buttons;
  int switches;
  std::transform(cfg_file.begin(), cfg_file.end(), cfg_file.begin(),
                   [](unsigned char c) { return (c == '-') ? '_' : toupper(c); });
  std::transform(cfg_file.begin(), cfg_file.end(), cfg_file.begin(),
                   [](unsigned char c) { return (c == '.') ? '_' : c; });

  search_replace_all(cfg_file, "/", "__");
  os << "#ifndef ASSEMBLY\n\n";
  os << "#ifndef " << cfg_file << "\n";
  os << "#define " << cfg_file << "\n\n";

  auto emit_comment = [&](const node &n) {
    os << "/* From " << n.name() << " */\n";
  };

  auto emit_handle = [&](std::string d, const node &n, std::string v) {
    emit_comment(n);
    os << "#define __MEE_DTB_HANDLE_" << d << "_" << n.handle() << " " << v << "\n";
  };

  auto emit_def_handle = [&](std::string handle, const node &n, std::string field) {
    emit_comment(n);
    os << "#define " << handle << " (&__mee_dt_" << n.handle() << field << ")\n\n";
    os << "#define " << "__MEE_DT_" << n.handle_cap() << "_HANDLE (&__mee_dt_"
       << n.handle() << field << ")\n\n";
  };

  auto emit_def = [&](std::string handle, std::string field) {
    os << "#define " << handle << " " << field << "\n\n";
  };

  auto emit_def_value = [&](std::string name, const node &n, std::string field) {
    os << "#define __MEE_" << n.handle_cap() << "_INTERRUPTS \t\t";
    if (name.compare("interrupts") == 0)
      os << n.get_fields_count<uint32_t>(name) << "\n";
    else
      os << n.get_fields_count<std::tuple<node, uint32_t>>(name) << "\n";
    os << "#define MEE_MAX_" << field << "\t __MEE_" << n.handle_cap() << "_INTERRUPTS\n\n";
  };

  std::set<std::string> included;
  auto emit_include = [&](std::string d) {
    if (included.find(d) != included.end())
      return;
    os << "#include <mee/drivers/" << d << ".h>\n";
    included.insert(d);
  };

  auto emit_struct_pointer_begin = [&](std::string type, std::string name, std::string ext) {
    os << "asm (\".weak " << name << "\");\n";
    os << "struct __mee_driver_" << type << " *" << name << ext << " = {\n";
  };
  auto emit_struct_pointer_element = [&](std::string type, uint32_t id,
					 std::string field, std::string delimiter) {
    os << "\t\t\t\t\t&__mee_dt_" << type << "_" << id << field << delimiter;
  };
  auto emit_struct_pointer_end = [&](std::string empty) {
    os << "\t\t\t\t\t" << empty << " };\n";
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

  auto emit_struct_container_node_and_array = [&](int size, std::string field1,
						  const node& c, std::string subfield1,
						  std::string field2, uint32_t elem) {
    static int cna = 0;
    if (cna == 0) {
        os << "    ." << field1 << " = &" << "__mee_dt_" << c.handle() << subfield1 << ",\n";
    }
    os << "    ." << field2 << "[" << cna << "] = " << elem << ",\n";
    cna++;
    if (cna == size) {
	cna = 0;
    }
  };

  auto emit_struct_field_array_elem = [&](int idx, std::string field, uint32_t elem) {
    os << "    ." << field << "[" << idx << "] = " << elem << ",\n";
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

  /* Let defines some constants, lie number of interrupt lines required in headers. */
  dtb.match(
    std::regex("riscv,clint0"),             [&](node n) { emit_def_value("interrupts-extended", n, "CLINT_INTERRUPTS"); },
    std::regex("riscv,plic0"),              [&](node n) { emit_def_value("interrupts-extended", n, "PLIC_INTERRUPTS"); },
    std::regex("sifive,local-external-interrupts0"),
                                            [&](node n) { emit_def_value("interrupts", n, "LOCAL_EXT_INTERRUPTS"); },
    std::regex("sifive,global-external-interrupts0"),
                                            [&](node n) { emit_def_value("interrupts", n, "GLOBAL_EXT_INTERRUPTS"); },
    std::regex("sifive,pwm"),               [&](node n) { emit_def_value("interrupts", n, "PWM_INTERRUPTS"); },
    std::regex("sifive,gpio0"),             [&](node n) { emit_def_value("interrupts", n, "GPIO_INTERRUPTS"); },
    std::regex("sifive,uart0"),             [&](node n) { emit_def_value("interrupts", n, "UART_INTERRUPTS"); }
  );

  /* First, find the required headers that must be included in order to make
   * this a sane MEE instance. */
  dtb.match(
    std::regex("cpu"),                      [&](node n) { emit_include("riscv,cpu");                },
    std::regex("riscv,clint0"),             [&](node n) { emit_include("riscv,clint0");             },
    std::regex("riscv,plic0"),              [&](node n) { emit_include("riscv,plic0");              },
    std::regex("fixed-clock"),              [&](node n) { emit_include("fixed-clock");              },
    std::regex("sifive,fe310-g000,pll"),    [&](node n) { emit_include("sifive,fe310-g000,pll");    },
    std::regex("sifive,fe310-g000,prci"),   [&](node n) { emit_include("sifive,fe310-g000,prci");   },
    std::regex("sifive,fe310-g000,hfxosc"), [&](node n) { emit_include("sifive,fe310-g000,hfxosc"); },
    std::regex("sifive,fe310-g000,hfrosc"), [&](node n) { emit_include("sifive,fe310-g000,hfrosc"); },
    std::regex("sifive,gpio0"),             [&](node n) { emit_include("sifive,gpio0");             },
    std::regex("sifive,uart0"),             [&](node n) { emit_include("sifive,uart0");             },
    std::regex("sifive,local-external-interrupts0"), [&](node n) { emit_include("sifive,local-external-interrupts0"); },
    std::regex("sifive,global-external-interrupts0"), [&](node n) { emit_include("sifive,global-external-interrupts0"); },
    std::regex("sifive,gpio-leds"),         [&](node n) { emit_include("sifive,gpio-leds"); },
    std::regex("sifive,gpio-buttons"),      [&](node n) { emit_include("sifive,gpio-buttons"); },
    std::regex("sifive,gpio-switches"),     [&](node n) { emit_include("sifive,gpio-switches"); },
    std::regex("sifive,test0"),             [&](node n) { emit_include("sifive,test0");             }
  );

  /* Now emit the various nodes's header definitons. */
  dtb.match(
    std::regex("cpu"),                      [&](node n) { emit_struct_decl("cpu",                      n); },
    std::regex("riscv,clint0"),             [&](node n) { emit_struct_decl("riscv_clint0",             n); },
    std::regex("riscv,plic0"),              [&](node n) { emit_struct_decl("riscv_plic0",              n); },
    std::regex("riscv,cpu-intc"),           [&](node n) { emit_struct_decl("riscv_cpu_intc",           n); },
    std::regex("fixed-clock"),              [&](node n) { emit_struct_decl("fixed_clock",              n); },
    std::regex("sifive,local-external-interrupts0"),
                                            [&](node n) { emit_struct_decl("sifive_local_external_interrupts0", n); },
    std::regex("sifive,global-external-interrupts0"),
					    [&](node n) { emit_struct_decl("sifive_global_external_interrupts0", n); },
    std::regex("sifive,fe310-g000,pll"),    [&](node n) { emit_struct_decl("sifive_fe310_g000_pll",    n); },
    std::regex("sifive,fe310-g000,prci"),   [&](node n) { emit_struct_decl("sifive_fe310_g000_prci",   n); },
    std::regex("sifive,fe310-g000,hfxosc"), [&](node n) { emit_struct_decl("sifive_fe310_g000_hfxosc", n); },
    std::regex("sifive,fe310-g000,hfrosc"), [&](node n) { emit_struct_decl("sifive_fe310_g000_hfrosc", n); },
    std::regex("sifive,gpio0"),             [&](node n) { emit_struct_decl("sifive_gpio0",             n); },
    std::regex("sifive,uart0"),             [&](node n) { emit_struct_decl("sifive_uart0",             n); },
    std::regex("sifive,gpio-leds"),         [&](node n) { emit_struct_decl("sifive_gpio_led",          n); },
    std::regex("sifive,gpio-buttons"),      [&](node n) { emit_struct_decl("sifive_gpio_button",       n); },
    std::regex("sifive,gpio-switches"),     [&](node n) { emit_struct_decl("sifive_gpio_switch",       n); },
    std::regex("sifive,test0"),             [&](node n) { emit_struct_decl("sifive_test0",             n); }
  );

  /* Walk through the device tree, emitting various nodes as we know about
   * them. */
  cpus = 0;
  leds = 0;
  buttons = 0;
  switches = 0;
  dtb.match(
    std::regex("cpu"), [&](node n) {
      emit_struct_begin("cpu", n);
      emit_struct_field("vtable", "&__mee_driver_vtable_cpu");
      emit_struct_field("cpu.vtable", "&__mee_driver_vtable_cpu.cpu_vtable");
      emit_struct_field_u32("timebase", n.get_field<uint32_t>("timebase-frequency"));
      emit_struct_field("interrupt_controller", "&__mee_dt_interrupt_controller.controller");
      emit_struct_end();
      emit_def_handle("__MEE_DT_RISCV_CPU_HANDLE", n, ".cpu");
      cpus++;
    }, std::regex("fixed-clock"), [&](node n) {
      emit_struct_begin("fixed_clock", n);
      emit_struct_field("vtable", "&__mee_driver_vtable_fixed_clock");
      emit_struct_field("clock.vtable", "&__mee_driver_vtable_fixed_clock.clock");
      emit_struct_field_u32("rate", n.get_field<uint32_t>("clock-frequency"));
      emit_struct_end();
    }, std::regex("riscv,cpu-intc"), [&](node n) {
      emit_struct_begin("riscv_cpu_intc", n);
      emit_struct_field("vtable", "&__mee_driver_vtable_riscv_cpu_intc");
      emit_struct_field("controller.vtable", "&__mee_driver_vtable_riscv_cpu_intc.controller_vtable");
      emit_struct_field("init_done", "0");
      if (n.field_exists("interrupt-controller")) {
          emit_struct_field("interrupt_controller", "1");
      }
      emit_struct_end();
      emit_def_handle("__MEE_DT_RISCV_CPU_INTC_HANDLE", n, ".controller");
    }, std::regex("riscv,clint0"), [&](node n) {
      emit_struct_begin("riscv_clint0", n);
      emit_struct_field("vtable", "&__mee_driver_vtable_riscv_clint0");
      emit_struct_field("clint.vtable", "&__mee_driver_vtable_riscv_clint0.clint_vtable");
      n.named_tuples(
        "reg-names", "reg",
        "control", tuple_t<target_addr, target_size>(), [&](target_addr base, target_size size) {
          emit_struct_field_ta("control_base", base);
          emit_struct_field_ts("control_size", size);
        });
      emit_struct_field("init_done", "0");
      emit_struct_field("num_interrupts", "MEE_MAX_CLINT_INTERRUPTS");
      n.maybe_tuple_size(
        "interrupts-extended", tuple_t<node, uint32_t>(),
        [&](){
	    emit_struct_field_null("interrupt_parent");
	    emit_struct_field("interrupt_lines[0]", "0");
	},
        [&](int s, node c, uint32_t line) {
            emit_struct_container_node_and_array(s, "interrupt_parent", c, ".controller",
						 "interrupt_lines", line);
        });
      emit_struct_end();
      emit_def_handle("__MEE_DT_RISCV_CLINT0_HANDLE", n, ".clint");
    }, std::regex("sifive,local-external-interrupts0"), [&](node n) {
      emit_struct_begin("sifive_local_external_interrupts0", n);
      emit_struct_field("vtable", "&__mee_driver_vtable_sifive_local_external_interrupts0");
      emit_struct_field("irc.vtable", "&__mee_driver_vtable_sifive_local_external_interrupts0.local0_vtable");
      emit_struct_field("init_done", "0");
      n.maybe_tuple(
        "interrupt-parent", tuple_t<node>(),
        [&](){ emit_struct_field_null("interrupt_parent"); },
        [&](node n) { emit_struct_field_node("interrupt_parent", n, ".controller"); });
      emit_struct_field("num_interrupts", "MEE_MAX_LOCAL_EXT_INTERRUPTS");
      n.maybe_tuple_index(
        "interrupts", tuple_t<uint32_t>(),
        [&](){ emit_struct_field("interrupt_lines[0]", "0"); },
        [&](int i, uint32_t irline){ emit_struct_field_array_elem(i, "interrupt_lines", irline); });
      emit_struct_end();
      emit_def_handle("__MEE_DT_SIFIVE_LOCAL_EXINTR0_HANDLE", n, ".irc");
    }, std::regex("riscv,plic0"), [&](node n) {
      emit_struct_begin("riscv_plic0", n);
      emit_struct_field("vtable", "&__mee_driver_vtable_riscv_plic0");
      emit_struct_field("plic0.vtable", "&__mee_driver_vtable_riscv_plic0.controller_vtable");
      emit_struct_field("init_done", "0");
      n.maybe_tuple(
        "interrupts-extended", tuple_t<node, uint32_t>(),
        [&](){
            emit_struct_field_null("interrupt_parent");
            emit_struct_field("interrupt_line", "");
        },
        [&](node n, uint32_t line) {
            emit_struct_field_node("interrupt_parent", n, ".controller");
            emit_struct_field_u32("interrupt_line", line);
        });
      n.named_tuples(
        "reg-names", "reg",
        "control", tuple_t<target_addr, target_size>(), [&](target_addr base, target_size size) {
          emit_struct_field_ta("control_base", base);
          emit_struct_field_ts("control_size", size);
        });
      emit_struct_field_u32("max_priority", n.get_field<uint32_t>("riscv,max-priority"));
      emit_struct_field_u32("num_interrupts", n.get_field<uint32_t>("riscv,ndev"));
      if (n.field_exists("interrupt-controller")) { 
          emit_struct_field("interrupt_controller", "1");
      }
      emit_struct_end();
      emit_def_handle("__MEE_DT_RISCV_PLIC0_HANDLE", n, ".plic0");
    }, std::regex("sifive,global-external-interrupts0"), [&](node n) {
      emit_struct_begin("sifive_global_external_interrupts0", n);
      emit_struct_field("vtable", "&__mee_driver_vtable_sifive_global_external_interrupts0");
      emit_struct_field("irc.vtable", "&__mee_driver_vtable_sifive_global_external_interrupts0.global0_vtable");
      emit_struct_field("init_done", "0");
      n.maybe_tuple(
        "interrupt-parent", tuple_t<node>(),
        [&](){ emit_struct_field_null("interrupt_parent"); },
        [&](node n) { emit_struct_field_node("interrupt_parent", n, ".plic0"); });
      emit_struct_field("num_interrupts", "MEE_MAX_GLOBAL_EXT_INTERRUPTS");
      n.maybe_tuple_index(
        "interrupts", tuple_t<uint32_t>(),
        [&](){ emit_struct_field("interrupt_lines[0]", "0"); },
        [&](int i, uint32_t irline){ emit_struct_field_array_elem(i, "interrupt_lines", irline); });
      emit_struct_end();
      emit_def_handle("__MEE_DT_SIFIVE_GLOBAL_EXINTR0_HANDLE", n, ".irc");
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
      n.maybe_tuple(
        "interrupt-parent", tuple_t<node>(),
        [&](){ emit_struct_field_null("interrupt_parent"); },
        [&](node n) { emit_struct_field_node("interrupt_parent", n, ".plic0"); });
      emit_struct_field("num_interrupts", "MEE_MAX_GPIO_INTERRUPTS");
      n.maybe_tuple_index(
        "interrupts", tuple_t<uint32_t>(),
        [&](){ emit_struct_field("interrupt_lines[0]", "0"); },
        [&](int i, uint32_t irline){ emit_struct_field_array_elem(i, "interrupt_lines", irline); });
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
      n.maybe_tuple(
        "interrupt-parent", tuple_t<node>(),
        [&](){ emit_struct_field_null("interrupt_parent"); },
        [&](node n) { emit_struct_field_node("interrupt_parent", n, ".plic0"); });
      emit_struct_field("num_interrupts", "MEE_MAX_UART_INTERRUPTS");
      n.maybe_tuple(
        "interrupts", tuple_t<uint32_t>(),
        [&](){ },
        [&](uint32_t line){ emit_struct_field_u32("interrupt_line", line); });
      emit_struct_end();
    }, std::regex("sifive,gpio-leds"), [&](node n) {
      emit_struct_begin("sifive_gpio_led", n);
      emit_struct_field("vtable", "&__mee_driver_vtable_sifive_led");
      emit_struct_field("led.vtable", "&__mee_driver_vtable_sifive_led.led_vtable");
      n.maybe_tuple(
        "gpios", tuple_t<node, uint32_t>(),
        [&](){
            emit_struct_field_null("gpio");
            emit_struct_field("pin", "0");
        },
        [&](node n, uint32_t line) {
            emit_struct_field_node("gpio", n, "");
            emit_struct_field_u32("pin", line);
        });
      emit_struct_field("label", "\"" + n.get_field<string>("label") + "\"");
      emit_struct_end();
      leds++;
    }, std::regex("sifive,gpio-buttons"), [&](node n) {
      emit_struct_begin("sifive_gpio_button", n);
      emit_struct_field("vtable", "&__mee_driver_vtable_sifive_button");
      emit_struct_field("button.vtable", "&__mee_driver_vtable_sifive_button.button_vtable");
      n.maybe_tuple(
        "gpios", tuple_t<node, uint32_t>(),
        [&](){
            emit_struct_field_null("gpio");
            emit_struct_field("pin", "0");
        },
        [&](node n, uint32_t line) {
            emit_struct_field_node("gpio", n, "");
            emit_struct_field_u32("pin", line);
        });
      n.maybe_tuple(
        "interrupts-extended", tuple_t<node, uint32_t>(),
        [&](){
            emit_struct_field_null("interrupt_parent");
            emit_struct_field("interrupt_line", "0");
        },
        [&](node n, uint32_t line) {
            emit_struct_field_node("interrupt_parent", n, ".irc");
            emit_struct_field_u32("interrupt_line", line);
        });
      emit_struct_field("label", "\"" + n.get_field<string>("label") + "\"");
      emit_struct_end();
      buttons++;
    }, std::regex("sifive,gpio-switches"), [&](node n) {
      emit_struct_begin("sifive_gpio_switch", n);
      emit_struct_field("vtable", "&__mee_driver_vtable_sifive_switch");
      emit_struct_field("flip.vtable", "&__mee_driver_vtable_sifive_switch.switch_vtable");
      n.maybe_tuple(
        "interrupts-extended", tuple_t<node, uint32_t>(),
        [&](){
            emit_struct_field_null("interrupt_parent");
            emit_struct_field("interrupt_line", "0");
        },
        [&](node n, uint32_t line) {
            emit_struct_field_node("interrupt_parent", n, ".irc");
            emit_struct_field_u32("interrupt_line", line);
        });
      emit_struct_field("label", "\"" + n.get_field<string>("label") + "\"");
      emit_struct_end();
      switches++;
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

  /* Create a list of cpus */
  emit_def("__MEE_DT_MAX_HARTS", std::to_string(cpus));
  emit_struct_pointer_begin("cpu", "__mee_cpu_table", "[]");
  for (int i=0; i < cpus; i++) {
    emit_struct_pointer_element("cpu", i, "",
				((i + 1) == cpus) ? "};\n\n" : ",\n");
  }
  emit_def("__MEE_DT_MAX_LEDS", std::to_string(leds));
  emit_struct_pointer_begin("sifive_gpio_led", "__mee_led_table", "[]");
  if (leds) {
    for (int i=0; i < leds; i++) {
      emit_struct_pointer_element("led", i/3,
				((i % 3) == 0) ? "red" : ((i % 3) == 1) ? "green" : "blue",
                                ((i + 1) == leds) ? "};\n\n" : ",\n");
    }
  } else {
    emit_struct_pointer_end("NULL");
  }
  emit_def("__MEE_DT_MAX_BUTTONS", std::to_string(buttons));
  emit_struct_pointer_begin("sifive_gpio_button", "__mee_button_table", "[]");
  if (buttons) {
    for (int i=0; i < buttons; i++) {
      emit_struct_pointer_element("button", i, "",
                                ((i + 1) == buttons) ? "};\n\n" : ",\n");
    }
  } else {
    emit_struct_pointer_end("NULL");
  }
  emit_def("__MEE_DT_MAX_SWITCHES", std::to_string(switches));
  emit_struct_pointer_begin("sifive_gpio_switch", "__mee_switch_table", "[]");
  if (switches) {
    for (int i=0; i < switches; i++) {
      emit_struct_pointer_element("switch", i, "",
                                ((i + 1) == switches) ? "};\n\n" : ",\n");
    }
  } else {
    emit_struct_pointer_end("NULL");
  }

  os << "#endif /*MEE__MACHINE__" << cfg_file << "*/\n\n";
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

    write_config_file(f, cfg, config_file);
  }

  return 0;
}
