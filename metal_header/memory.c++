/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <memory.h>

memory::memory(std::ostream &os, const fdt &dtb) : Device(os, dtb, "") {
  num_memories = 0;
  dtb.match(std::regex("sifive,sram0"), [&](node n) { num_memories += 1; },
            std::regex("sifive,testram0"), [&](node n) { num_memories += 1; },
            std::regex("sifive,dtim0"), [&](node n) { num_memories += 1; },
            std::regex("sifive,itim0"), [&](node n) { num_memories += 1; },
            std::regex("memory"), [&](node n) { num_memories += 1; },
            std::regex("sifive,spi0"),
            [&](node n) {
              /* Increment if one of the reg-names is "mem" */
              n.named_tuples("reg-names", "reg", "mem",
                             tuple_t<target_addr, target_size>(),
                             [&](target_addr base, target_size size) {
                               num_memories += 1;
                             });
            });
}

void memory::include_headers() { os << "#include <metal/memory.h>\n"; }

void memory::declare_structs() {
  auto declare = [&](node n) {
    os << "extern struct metal_memory __metal_dt_mem_" << n.handle() << ";\n\n";
  };

  dtb.match(std::regex("sifive,sram0"), declare, std::regex("sifive,testram0"),
            declare, std::regex("sifive,dtim0"), declare,
            std::regex("sifive,itim0"), declare, std::regex("memory"), declare,
            std::regex("sifive,spi0"), declare);
}

void memory::define_structs() {
  auto define = [&](node n) {
    os << "struct metal_memory __metal_dt_mem_" << n.handle() << " = {\n";

    if (n.field_exists("reg-names")) {
      n.named_tuples("reg-names", "reg", "mem",
                     tuple_t<target_addr, target_size>(),
                     [&](target_addr base, target_size size) {
                       emit_struct_field_ta("_base_address", base);
                       emit_struct_field_ts("_size", size);
                     });
    } else {
      auto regs = n.get_fields<std::tuple<target_addr, target_size>>("reg");

      if (!regs.empty()) {
        emit_struct_field_ta("_base_address", std::get<0>(regs.front()));
        emit_struct_field_ts("_size", std::get<1>(regs.front()));
      }
    }

    n.maybe_tuple("attributes", tuple_t<string>(),
                  [&]() {
                    os << "    ._attrs = {\n";
                    os << "        .R = 1,\n";
                    os << "        .W = 1,\n";
                    os << "        .X = 1,\n";
                    os << "        .C = 1,\n";
                    os << "        .A = 1},\n";
                  },
                  [&](string attributes) {
                    int R = 0;
                    int W = 0;
                    int X = 0;
                    int C = 0;
                    int A = 0;

                    if (attributes.find("r") != string::npos) {
                      R = 1;
                    }
                    if (attributes.find("w") != string::npos) {
                      W = 1;
                    }
                    if (attributes.find("x") != string::npos) {
                      X = 1;
                    }
                    if (attributes.find("c") != string::npos) {
                      C = 1;
                    }
                    if (attributes.find("a") != string::npos) {
                      A = 1;
                    }

                    os << "    ._attrs = {\n";
                    os << "        .R = " << std::to_string(R) << ",\n";
                    os << "        .W = " << std::to_string(W) << ",\n";
                    os << "        .X = " << std::to_string(X) << ",\n";
                    os << "        .C = " << std::to_string(C) << ",\n";
                    os << "        .A = " << std::to_string(A) << "},\n";
                  });

    emit_struct_end();
  };

  dtb.match(std::regex("sifive,sram0"), define, std::regex("sifive,testram0"),
            define, std::regex("sifive,dtim0"), define,
            std::regex("sifive,itim0"), define, std::regex("memory"), define,
            std::regex("sifive,spi0"), define);
}

void memory::create_handles() {
  emit_def("__METAL_DT_MAX_MEMORIES", std::to_string(num_memories));

  os << "__asm__ (\".weak __metal_memory_table\");\n";
  os << "struct metal_memory *__metal_memory_table[] = {\n";

  int i = 0;
  auto emit = [&](node n) {
    os << "\t\t\t\t\t&__metal_dt_mem_" << n.handle();

    if ((i + 1) == num_memories) {
      os << "};\n\n";
    } else {
      os << ",\n";
    }

    i += 1;
  };

  if (num_memories) {
    dtb.match(
        std::regex("sifive,sram0"), emit, std::regex("sifive,testram0"), emit,
        std::regex("sifive,dtim0"), emit, std::regex("sifive,itim0"), emit,
        std::regex("memory"), emit, std::regex("sifive,spi0"), [&](node n) {
          /* Emit if one of the reg-names is "mem" */
          n.named_tuples("reg-names", "reg", "mem",
                         tuple_t<target_addr, target_size>(),
                         [&](target_addr base, target_size size) { emit(n); });
        });

  } else {
    emit_struct_pointer_end("NULL");
  }
}
