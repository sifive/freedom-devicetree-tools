/* Copyright 2021 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <sifive_remapper2.h>

#include <regex>

using std::string;

sifive_remapper2::sifive_remapper2(std::ostream &os, const fdt &dtb)
    : Device(os, dtb, "sifive,remapper2") {}

void sifive_remapper2::create_defines() {
  dtb.match(std::regex(compat_string), [&](node n) {
    uint32_t low, high;
    uint64_t from_region_base, from_region_size;
    uint64_t to_region_base, to_region_size;
    uint32_t max_from_entry_addr_width;

    low = n.get_fields<uint32_t>("sifive,remapper-frm-region-base").at(1);
    high = n.get_fields<uint32_t>("sifive,remapper-frm-region-base").at(0);
    from_region_base = ((uint64_t)high << 32) | low;
    emit_def(platform_define(n, "FROM_REGION_BASE"),
             std::to_string(from_region_base));

    low = n.get_fields<uint32_t>("sifive,remapper-frm-region-size").at(1);
    high = n.get_fields<uint32_t>("sifive,remapper-frm-region-size").at(0);
    from_region_size = ((uint64_t)high << 32) | low;
    emit_def(platform_define(n, "FROM_REGION_SIZE"),
             std::to_string(from_region_size));

    low = n.get_fields<uint32_t>("sifive,remapper-to-region-base").at(1);
    high = n.get_fields<uint32_t>("sifive,remapper-to-region-base").at(0);
    to_region_base = ((uint64_t)high << 32) | low;
    emit_def(platform_define(n, "TO_REGION_BASE"),
             std::to_string(to_region_base));

    low = n.get_fields<uint32_t>("sifive,remapper-to-region-size").at(1);
    high = n.get_fields<uint32_t>("sifive,remapper-to-region-size").at(0);
    to_region_size = ((uint64_t)high << 32) | low;
    emit_def(platform_define(n, "TO_REGION_SIZE"),
             std::to_string(to_region_size));

    max_from_entry_addr_width =
        n.get_field<uint32_t>("sifive,remapper-max-frm-width");
    emit_def(platform_define(n, "MAX_FROM_ENTRY_REGION_ADDR_WIDTH"),
             std::to_string(max_from_entry_addr_width));
  });
}

void sifive_remapper2::include_headers() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_include(compat_string); });
}

void sifive_remapper2::declare_inlines() {
  Inline *func;
  std::list<Inline *> extern_inlines;
  int count = 0;

  dtb.match(std::regex(compat_string), [&](node n) {
    n.named_tuples(
        "reg-names", "reg", "control", tuple_t<target_addr, target_size>(),
        [&](target_addr base, target_size size) {
          if (count == 0) {
            func = create_inline_dec("base", "unsigned long",
                                     "const struct metal_remapper *remapper");
            extern_inlines.push_back(func);

            func = create_inline_dec("size", "unsigned long",
                                     "const struct metal_remapper *remapper");
            extern_inlines.push_back(func);
            func = create_inline_dec("from_region_base", "unsigned long long",
                                     "const struct metal_remapper *remapper");
            extern_inlines.push_back(func);
            func = create_inline_dec("from_region_size", "unsigned long long",
                                     "const struct metal_remapper *remapper");
            extern_inlines.push_back(func);
            func = create_inline_dec("to_region_base", "unsigned long long",
                                     "const struct metal_remapper *remapper");
            extern_inlines.push_back(func);
            func = create_inline_dec("to_region_size", "unsigned long long",
                                     "const struct metal_remapper *remapper");
            extern_inlines.push_back(func);
            func =
                create_inline_dec("max_from_entry_addr_width", "unsigned int",
                                  "const struct metal_remapper *remapper");
            extern_inlines.push_back(func);
          }
        });
    count++;
  });
  os << "\n";
  os << "/* --------------------- sifive_remapper2 ------------ */\n";
  while (!extern_inlines.empty()) {
    func = extern_inlines.front();
    extern_inlines.pop_front();
    emit_inline_dec(func, "sifive_remapper2");
    delete func;
  }
  os << "\n";
}

void sifive_remapper2::define_inlines() {
  Inline *base_func;
  Inline *size_func;
  Inline *from_region_base_func;
  Inline *from_region_size_func;
  Inline *to_region_base_func;
  Inline *to_region_size_func;
  Inline *max_from_entry_addr_width_func;

  int count = 0;

  dtb.match(std::regex(compat_string), [&](node n) {
    if (count == 0) {
      base_func = create_inline_def(
          "base", "unsigned long",
          "(uintptr_t)remapper == (uintptr_t)&__metal_dt_" + n.handle(),
          platform_define(n, METAL_BASE_ADDRESS_LABEL),
          "const struct metal_remapper *remapper");

      size_func = create_inline_def(
          "size", "unsigned long",
          "(uintptr_t)remapper == (uintptr_t)&__metal_dt_" + n.handle(),
          platform_define(n, METAL_SIZE_LABEL),
          "const struct metal_remapper *remapper");

      from_region_base_func = create_inline_def(
          "from_region_base", "unsigned long long",
          "(uintptr_t)remapper == (uintptr_t)&__metal_dt_" + n.handle(),
          platform_define(n, "FROM_REGION_BASE"),
          "const struct metal_remapper *remapper");

      from_region_size_func = create_inline_def(
          "from_region_size", "unsigned long long",
          "(uintptr_t)remapper == (uintptr_t)&__metal_dt_" + n.handle(),
          platform_define(n, "FROM_REGION_SIZE"),
          "const struct metal_remapper *remapper");

      to_region_base_func = create_inline_def(
          "to_region_base", "unsigned long long",
          "(uintptr_t)remapper == (uintptr_t)&__metal_dt_" + n.handle(),
          platform_define(n, "TO_REGION_BASE"),
          "const struct metal_remapper *remapper");

      to_region_size_func = create_inline_def(
          "to_region_size", "unsigned long long",
          "(uintptr_t)remapper == (uintptr_t)&__metal_dt_" + n.handle(),
          platform_define(n, "TO_REGION_SIZE"),
          "const struct metal_remapper *remapper");
      max_from_entry_addr_width_func = create_inline_def(
          "max_from_entry_addr_width", "unsigned int",
          "(uintptr_t)remapper == (uintptr_t)&__metal_dt_" + n.handle(),
          platform_define(n, "MAX_FROM_ENTRY_REGION_ADDR_WIDTH"),
          "const struct metal_remapper *remapper");
    } else {
      add_inline_body(base_func,
                      "(uintptr_t)remapper == (uintptr_t)&__metal_dt_" +
                          n.handle(),
                      platform_define(n, METAL_BASE_ADDRESS_LABEL));
      add_inline_body(size_func,
                      "(uintptr_t)remapper == (uintptr_t)&__metal_dt_" +
                          n.handle(),
                      platform_define(n, METAL_SIZE_LABEL));
      add_inline_body(from_region_base_func,
                      "(uintptr_t)remapper == (uintptr_t)&__metal_dt_" +
                          n.handle(),
                      platform_define(n, "FROM_REGION_BASE"));
      add_inline_body(from_region_size_func,
                      "(uintptr_t)remapper == (uintptr_t)&__metal_dt_" +
                          n.handle(),
                      platform_define(n, "FROM_REGION_SIZE"));
      add_inline_body(to_region_base_func,
                      "(uintptr_t)remapper == (uintptr_t)&__metal_dt_" +
                          n.handle(),
                      platform_define(n, "TO_REGION_BASE"));
      add_inline_body(to_region_size_func,
                      "(uintptr_t)remapper == (uintptr_t)&__metal_dt_" +
                          n.handle(),
                      platform_define(n, "TO_REGION_SIZE"));
      add_inline_body(max_from_entry_addr_width_func,
                      "(uintptr_t)remapper == (uintptr_t)&__metal_dt_" +
                          n.handle(),
                      platform_define(n, "MAX_FROM_ENTRY_REGION_ADDR_WIDTH"));
    }

    count += 1;
  });
  os << "\n";
  os << "/* --------------------- sifive_remapper2 ------------ */\n";
  if (count != 0) {
    add_inline_body(base_func, "else", "0");
    emit_inline_def(base_func, "sifive_remapper2");
    delete base_func;
    add_inline_body(size_func, "else", "0");
    emit_inline_def(size_func, "sifive_remapper2");
    delete size_func;
    add_inline_body(from_region_base_func, "else", "0");
    emit_inline_def(from_region_base_func, "sifive_remapper2");
    delete from_region_base_func;
    add_inline_body(from_region_size_func, "else", "0");
    emit_inline_def(from_region_size_func, "sifive_remapper2");
    delete from_region_size_func;
    add_inline_body(to_region_base_func, "else", "0");
    emit_inline_def(to_region_base_func, "sifive_remapper2");
    delete to_region_base_func;
    add_inline_body(to_region_size_func, "else", "0");
    emit_inline_def(to_region_size_func, "sifive_remapper2");
    delete to_region_size_func;
    add_inline_body(max_from_entry_addr_width_func, "else", "0");
    emit_inline_def(max_from_entry_addr_width_func, "sifive_remapper2");
  }
  os << "\n";
}

void sifive_remapper2::declare_structs() {
  dtb.match(std::regex(compat_string),
            [&](node n) { emit_struct_decl("sifive_remapper2", n); });
}

void sifive_remapper2::define_structs() {
  dtb.match(std::regex(compat_string), [&](node n) {
    emit_struct_begin("sifive_remapper2", n);

    emit_struct_field("remapper.vtable",
                      "&__metal_driver_vtable_sifive_remapper2.remapper");

    emit_struct_end();
  });
}

void sifive_remapper2::create_handles() {
  dtb.match(std::regex(compat_string), [&](node n) {
    emit_def_handle("__METAL_DT_REMAPPER_HANDLE", n, ".remapper");
  });
}
