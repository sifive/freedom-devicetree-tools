/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <sifive_fe310_g000_lfrosc.h>

#include <regex>

using std::string;

sifive_fe310_g000_lfrosc::sifive_fe310_g000_lfrosc(std::ostream &os,
                                                   const fdt &dtb)
    : Device(os, dtb, "sifive,fe310-g000,lfrosc") {}

void sifive_fe310_g000_lfrosc::include_headers() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_include(compat_string); });
}

void sifive_fe310_g000_lfrosc::declare_inlines() {
  Inline *func;
  std::list<Inline *> extern_inlines;
  int count = 0;

  dtb.match(std::regex(compat_string), [&](node n) {
    if (count == 0) {
      func = create_inline_dec("lfrosc", "struct metal_clock *",
                               "const struct metal_clock *clock");
      extern_inlines.push_back(func);

      func = create_inline_dec("psdlfaltclk", "struct metal_clock *",
                               "const struct metal_clock *clock");
      extern_inlines.push_back(func);

      func = create_inline_dec("config_reg", "unsigned long int",
                               "const struct metal_clock *clock");
      extern_inlines.push_back(func);

      func = create_inline_dec("mux_reg", "unsigned long int",
                               "const struct metal_clock *clock");
      extern_inlines.push_back(func);
    }
    count++;
  });
  os << "\n";
  os << "/* --------------------- sifive_fe310_g000_lfrosc ------------ */\n";
  while (!extern_inlines.empty()) {
    func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_dec(func, "sifive_fe310_g000_lfrosc");
    delete func;
  }
  os << "\n";
}

void sifive_fe310_g000_lfrosc::define_inlines() {
  Inline *lfrosc_func;
  Inline *psdlfaltclk_func;
  Inline *cfg_func;
  Inline *mux_func;

  int count = 0;

  dtb.match(std::regex(compat_string), [&](node n) {
    /* Input clock */
    string lfrosc_node = "NULL";
    string psdlfaltclk_node = "NULL";
    n.named_tuples("clock-names", "clocks", "lfrosc", tuple_t<node>(),
                   [&](node m) {
                     lfrosc_node = "(struct metal_clock *)&__metal_dt_" +
                                   m.handle() + ".clock";
                   },
                   "psdlfaltclk", tuple_t<node>(),
                   [&](node m) {
                     psdlfaltclk_node = "(struct metal_clock *)&__metal_dt_" +
                                        m.handle() + ".clock";
                   });

    /* Configuration base and offset */
    string config_reg = "0";
    string mux_reg = "0";
    n.named_tuples(
        "reg-names", "reg", "config", tuple_t<node, target_size>(),
        [&](node m, target_size offset) {
          uint64_t base = 0;
          m.named_tuples("reg-names", "reg", "control",
                         tuple_t<target_addr, target_size>(),
                         [&](target_addr b, target_size size) { base = b; });
          config_reg = std::to_string(base + offset);
        },
        "mux", tuple_t<node, target_size>(),
        [&](node m, target_size offset) {
          uint64_t base = 0;
          m.named_tuples("reg-names", "reg", "control",
                         tuple_t<target_addr, target_size>(),
                         [&](target_addr b, target_size size) { base = b; });
          mux_reg = std::to_string(base + offset);
        });

    if (count == 0) {
      lfrosc_func = create_inline_def(
          "lfrosc", "struct metal_clock *",
          "(uintptr_t)clock == (uintptr_t)&__metal_dt_" + n.handle(),
          lfrosc_node, "const struct metal_clock *clock");
      psdlfaltclk_func = create_inline_def(
          "psdlfaltclk", "struct metal_clock *",
          "(uintptr_t)clock == (uintptr_t)&__metal_dt_" + n.handle(),
          psdlfaltclk_node, "const struct metal_clock *clock");
      cfg_func = create_inline_def(
          "config_reg", "unsigned long int",
          "(uintptr_t)clock == (uintptr_t)&__metal_dt_" + n.handle(),
          config_reg, "const struct metal_clock *clock");
      mux_func = create_inline_def(
          "mux_reg", "unsigned long int",
          "(uintptr_t)clock == (uintptr_t)&__metal_dt_" + n.handle(), mux_reg,
          "const struct metal_clock *clock");
    } else {
      add_inline_body(lfrosc_func,
                      "(uintptr_t)clock == (uintptr_t)&__metal_dt_" +
                          n.handle(),
                      lfrosc_node);
      add_inline_body(psdlfaltclk_func,
                      "(uintptr_t)clock == (uintptr_t)&__metal_dt_" +
                          n.handle(),
                      psdlfaltclk_node);
      add_inline_body(
          cfg_func, "(uintptr_t)clock == (uintptr_t)&__metal_dt_" + n.handle(),
          config_reg);
      add_inline_body(
          mux_func, "(uintptr_t)clock == (uintptr_t)&__metal_dt_" + n.handle(),
          mux_reg);
    }

    count += 1;
  });

  os << "\n";
  os << "/* --------------------- sifive_fe310_g000_lfrosc ------------ */\n";

  if (count != 0) {
    add_inline_body(lfrosc_func, "else", "NULL");
    emit_inline_def(lfrosc_func, "sifive_fe310_g000_lfrosc");
    delete lfrosc_func;
    add_inline_body(psdlfaltclk_func, "else", "NULL");
    emit_inline_def(psdlfaltclk_func, "sifive_fe310_g000_lfrosc");
    delete psdlfaltclk_func;
    add_inline_body(cfg_func, "else", "0");
    emit_inline_def(cfg_func, "sifive_fe310_g000_lfrosc");
    delete cfg_func;
    add_inline_body(mux_func, "else", "0");
    emit_inline_def(mux_func, "sifive_fe310_g000_lfrosc");
    delete mux_func;
  }
  os << "\n";
}

void sifive_fe310_g000_lfrosc::declare_structs() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_struct_decl("sifive_fe310_g000_lfrosc", n); });
}

void sifive_fe310_g000_lfrosc::define_structs() {
  dtb.match(std::regex(compat_string), [&](node n) {
    emit_struct_begin("sifive_fe310_g000_lfrosc", n);

    emit_struct_field("clock.vtable",
                      "&__metal_driver_vtable_sifive_fe310_g000_lfrosc.clock");

    emit_struct_end();
  });
}
