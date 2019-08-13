/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <sifive_fe310_g000_pll.h>

#include <regex>

sifive_fe310_g000_pll::sifive_fe310_g000_pll(std::ostream &os, const fdt &dtb)
    : Device(os, dtb, "sifive,fe310-g000,pll") {}

void sifive_fe310_g000_pll::include_headers() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_include(compat_string); });
}

void sifive_fe310_g000_pll::declare_inlines() {
  Inline *func;
  std::list<Inline *> extern_inlines;
  int count = 0;

  dtb.match(std::regex(compat_string), [&](node n) {
    if (count == 0) {
      func = create_inline_dec("pllsel0", "struct metal_clock *",
                               "const struct metal_clock *clock");
      extern_inlines.push_back(func);

      func = create_inline_dec("pllref", "struct metal_clock *",
                               "const struct metal_clock *clock");
      extern_inlines.push_back(func);

      func = create_inline_dec(
          "config_base", "struct __metal_driver_sifive_fe310_g000_prci *", " ");
      extern_inlines.push_back(func);

      func = create_inline_dec("config_offset", "long", " ");
      extern_inlines.push_back(func);

      func = create_inline_dec("divider_base",
                               "struct __metal_driver_sifive_fe310_g000_prci *",
                               "const struct metal_clock *clock");
      extern_inlines.push_back(func);

      func = create_inline_dec("divider_offset", "long",
                               "const struct metal_clock *clock");
      extern_inlines.push_back(func);

      func = create_inline_dec("init_rate", "long", " ");
      extern_inlines.push_back(func);
    }
    count++;
  });
  os << "\n";
  os << "/* --------------------- sifive_fe310_g000_pll ------------ */\n";
  while (!extern_inlines.empty()) {
    func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_dec(func, "sifive_fe310_g000_pll");
    delete func;
  }
  os << "\n";
}

void sifive_fe310_g000_pll::define_inlines() {
  Inline *func;
  std::list<Inline *> extern_inlines;

  int count = 0;
  dtb.match(std::regex(compat_string), [&](node n) {
    n.named_tuples(
        "clock-names", "clocks", "pllref", tuple_t<node>(),
        [&](node m) {
          std::string value =
              "(struct metal_clock *)&__metal_dt_" + m.handle() + ".clock";
          func = create_inline_def("pllref", "struct metal_clock *", "empty",
                                   value, "const struct metal_clock *clock");
          extern_inlines.push_back(func);
        },
        "pllsel0", tuple_t<node>(),
        [&](node m) {
          std::string value =
              "(struct metal_clock *)&__metal_dt_" + m.handle() + ".clock";
          func = create_inline_def("pllsel0", "struct metal_clock *", "empty",
                                   value, "const struct metal_clock *clock");
          extern_inlines.push_back(func);
        });

    n.named_tuples(
        "reg-names", "reg", "config", tuple_t<node, target_size>(),
        [&](node m, target_size offset) {
          func = create_inline_def(
              "config_base", "struct __metal_driver_sifive_fe310_g000_prci *",
              "empty",
              "(struct __metal_driver_sifive_fe310_g000_prci *)&__metal_dt_" +
                  m.handle(),
              " ");
          extern_inlines.push_back(func);

          func = create_inline_def(
              "config_offset", "long", "empty",
              platform_define_offset(m, METAL_PLLCFG_LABEL), " ");
          extern_inlines.push_back(func);
        },
        "divider", tuple_t<node, target_size>(),
        [&](node m, target_size offset) {
          func = create_inline_def(
              "divider_base", "struct __metal_driver_sifive_fe310_g000_prci *",
              "empty",
              "(struct __metal_driver_sifive_fe310_g000_prci *)&__metal_dt_" +
                  m.handle(),
              "const struct metal_clock *clock");
          extern_inlines.push_back(func);

          func = create_inline_def(
              "divider_offset", "long", "empty",
              platform_define_offset(m, METAL_PLLOUTDIV_LABEL),
              "const struct metal_clock *clock");
          extern_inlines.push_back(func);
        });
    if (count == 0) {
      int freq = n.get_field<uint32_t>("clock-frequency");
      func = create_inline_def("init_rate", "long", "empty",
                               std::to_string(freq), " ");
      extern_inlines.push_back(func);
    }
    count++;
  });
  os << "\n";
  os << "/* --------------------- sifive_fe310_g000_pll ------------ */\n";
  while (!extern_inlines.empty()) {
    func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_def(func, "sifive_fe310_g000_pll");
    delete func;
  }
  os << "\n";
}

void sifive_fe310_g000_pll::declare_structs() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_struct_decl("sifive_fe310_g000_pll", n); });
}

void sifive_fe310_g000_pll::define_structs() {
  dtb.match(std::regex(compat_string), [&](node n) {
    emit_struct_begin("sifive_fe310_g000_pll", n);

    emit_struct_field("clock.vtable",
                      "&__metal_driver_vtable_sifive_fe310_g000_pll.clock");

    emit_struct_end();
  });
}

void sifive_fe310_g000_pll::create_handles() {
  dtb.match(std::regex(compat_string), [&](node n) {
    emit_def_handle("__METAL_DT_SIFIVE_FE310_G000_PLL_HANDLE", n, "");
  });
}
