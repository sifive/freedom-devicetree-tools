/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <fcntl.h>
#include <stdarg.h>
#include <stdlib.h>
#include <unistd.h>

#include <fstream>
#include <iostream>
#include <iterator>
#include <regex>
#include <sstream>
#include <string>
#include <vector>
#include <iomanip>
#include <ctime>

#include <fdt.h++>

using std::cout;
using std::endl;
using std::ifstream;
using std::ofstream;
using std::fstream;
using std::string;
class memory {
public:
  std::string mem_type;
  std::string mem_alias;
  std::string mem_name;
  uint64_t mem_start;
  uint64_t mem_length;

  memory() {}
  memory(std::string, std::string, std::string, uint64_t, uint64_t);
};

memory::memory (std::string mtype, std::string alias, std::string name, uint64_t start, uint64_t length)
{
  mem_type = mtype;
  mem_alias = alias;
  mem_name = name;
  mem_start = start;
  mem_length = length;
}

static void write_banner(fstream &os, std::string rel_tag)
{
  os << "/* Copyright 2019 SiFive, Inc */" << std::endl;
  os << "/* SPDX-License-Identifier: Apache-2.0 */" << std::endl;
  os << "/* ----------------------------------- */" << std::endl;
  if ( !rel_tag.empty() ) {
    auto t = std::time(nullptr);
    auto tm = *std::localtime(&t);
    os << "/* [" << rel_tag << "] "
       << std::put_time(&tm, "%d-%m-%Y %H-%M-%S") << "        */" << std::endl;
  }
  os << "/* ----------------------------------- */" << std::endl << std::endl;
}

static char *dts_blob;
static std::vector<memory> dts_memory_list;

static inline bool has_memory (std::string mem)
{
  for (auto entry : dts_memory_list) {
    if (entry.mem_name.find(mem) != std::string::npos) {
      std::cout << "has " << mem << std::endl;
      return true;
    }
  }
  return false;
}

static inline bool linker_has_memory (std::string mem)
{
  for (auto entry : dts_memory_list) {
    if (entry.mem_alias.compare(mem) == 0) {
      if (mem.compare("itim") == 0) {
        /* ITIM is special, if .text dont fit in 16KB skip it */
        if (entry.mem_length < 0x6000) {
           return false;
        }
      }
      std::cout << "is " << mem << std::endl;
      return true;
    }
  }
  return false;
}

static void alias_memory (std::string from, std::string to)
{
  std::vector<memory>::iterator it;

  for (it = dts_memory_list.begin(); it != dts_memory_list.end(); ++it) {
    if (it->mem_alias.compare(from) == 0) {
      it->mem_alias = to;
      std::cout << "alias " << from << " to " << it->mem_alias << std::endl;
    }
  }
}

/* Helper functions for extracting the number of address cells and size
 * cells for a node */

static uint64_t get_address_cells(const node &n) {
  if(n.field_exists("#address-cells")) {
    return n.get_field<uint32_t>("#address-cells");
  } else {
    /* The node::num_addr_cells() method gets the address cells of the parent
     * node */
    return n.num_addr_cells();
  }
}

static uint64_t get_size_cells(const node &n) {
  if(n.field_exists("#size-cells")) {
    return n.get_field<uint32_t>("#size-cells");
  } else {
    /* The node::num_size_cells() method gets the address cells of the parent
     * node */
    return n.num_size_cells();
  }
}

/* Helper functions for getting the child address and size from a ranges property */

static target_addr get_memory_ranges_address(const node &n) {
  int child_addr_cells = get_address_cells(n);

  auto cells = n.get_fields<uint32_t>("ranges");

  if (child_addr_cells == 1) {
    return cells.at(0);
  } else if (child_addr_cells == 2) {
    return ( (target_addr) cells.at(0) << 32) + (target_addr) cells.at(1);
  } else {
    std::cerr << "Node " << n.handle() << " has unsupported number of address cells " <<
      std::to_string(child_addr_cells) << std::endl;
  }

  return 0;
}

static target_size get_memory_ranges_size(const node &n) {
  int child_addr_cells = get_address_cells(n);
  int parent_addr_cells = get_address_cells(n.parent());
  int child_size_cells = get_size_cells(n);

  auto cells = n.get_fields<uint32_t>("ranges");

  if (child_size_cells == 1) {
    return cells.at(child_addr_cells + parent_addr_cells);
  } else if (child_size_cells == 2) {
    return ( (target_addr) cells.at(child_addr_cells + parent_addr_cells) << 32) + (target_addr) cells.at(child_addr_cells + parent_addr_cells + 1);
  } else {
    std::cerr << "Node " << n.handle() << " has unsupported number of size cells " <<
      std::to_string(child_size_cells) << std::endl;
  }

  return cells.back();
}

static void split_sram (std::string sram_orig)
{
  std::vector<memory>::iterator it;
  uint64_t split_mem_start = 0;
  uint64_t split_mem_length = 0;

  for (it = dts_memory_list.begin(); it != dts_memory_list.end(); ++it) {
    if (it->mem_alias.compare(sram_orig) == 0) {
      if (it->mem_length >= 0x10000) {
        it->mem_length = it->mem_length / 2;
        split_mem_start = it->mem_start + it->mem_length;
        split_mem_length = it->mem_length;
      } else
        std::cout << "sram size not big enough for spliting" << std::endl;
    }
  }
  if (split_mem_start != 0)
    dts_memory_list.push_back(memory("mem", "sram1", "sifive,sram0",
                split_mem_start, split_mem_length));
}

template<typename Out>
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

std::string trim(const std::string& str,
		 const std::string& whitespace = " \t")
{
  const auto strBegin = str.find_first_not_of(whitespace);
  if (strBegin == std::string::npos)
    return "";

  const auto strEnd = str.find_last_not_of(whitespace);
  const auto strRange = strEnd - strBegin + 1;

  return str.substr(strBegin, strRange);
}

static char *dts_read (const char *filename)
{
    char *buf = NULL;
    int fd = 0;	/* assume stdin */
    size_t bufsize = 1024, offset =0;;
    int ret = 0;

    if (strcmp(filename, "-") != 0) {
        fd = open(filename, O_RDONLY);
	if (fd < 0)
	    return buf;
    }

    /* Loop until we have read everything */
    buf = (char *)malloc(bufsize);
    do {
        /* Expand the buffer to hold the next chunk */
        if (offset == bufsize) {
	    bufsize *= 2;
	    buf = (char *)realloc(buf, bufsize);
	}

	ret = read(fd, &buf[offset], bufsize - offset);
	if (ret < 0) {
	    break;
	}
	offset += ret;
    } while (ret != 0);

    /* Clean up, including closing stdin; return errno on error */
    close(fd);
    if (ret) {
        free(buf);
	return NULL;
    }
    return buf;
}

static string get_dts_attribute (const string path, const string tag)
{
    int node, len;
    const char *value;

    node = fdt_path_offset(dts_blob, path.c_str());
    if (node < 0) return "";
    value = (const char *)fdt_getprop(dts_blob, node, tag.c_str(), &len);
    if (!value) return "";
    std::cout << string(value) << std::endl;
    return string(value);
}

static void dts_memory (bool ramrodata)
{
    if (dts_blob == nullptr)
        return;

    /* FIXME: This is particularly ugly, but we'll have to live with it for
     * now.  The idea is to avoid defining a single memory multiple times,
     * which will happen if there's multiple memories of a single type in a
     * design.  For example, multi-core designs will have multiple DTIMs. */
    int dtim_count = 0;
    int itim_count = 0;
    int sram_count = 0;
    int testram_count = 0;
    int spi_count = 0;
    int memory_count = 0;
    int ahb_periph_count = 0;
    int apb_periph_count = 0;
    int axi4_periph_count = 0;
    int tl_periph_count = 0;
    int ahb_sys_count = 0;
    int apb_sys_count = 0;
    int axi4_sys_count = 0;
    int tl_sys_count = 0;
    int cpu_count = 0;
    int dcache_size = 0;

    auto dtb = fdt((const uint8_t *)dts_blob);
    dtb.match(
        std::regex("cpu"), [&](node n) {
            auto name = n.name();
            /* Get dcache-size, 0 if not present */
            n.maybe_tuple(
                "d-cache-size", tuple_t<uint32_t>(),
                [&]() { dcache_size = 0; },
                [&](uint32_t num) { dcache_size = num; });
            cpu_count++;
        },
        std::regex("memory"), [&](node n) {
            auto name = n.name();
            n.maybe_tuple(
                "reg", tuple_t<target_addr, target_size>(),
                [&]() {},
		[&](target_addr base, target_size size) {
                    if (memory_count == 0)
                        dts_memory_list.push_back(memory("mem", "memory", "memory", base, size));
                    memory_count++;
                });
        },
        std::regex("sifive,dtim0"), [&](node n) {
            auto name = n.name();
            n.named_tuples(
                "reg-names", "reg",
                "mem", tuple_t<target_addr, target_size>(), [&](target_addr base, target_size size) {
                    if (dtim_count == 0)
                        dts_memory_list.push_back(memory("mem", "dtim", "sifive,dtim0", base, size));
                    dtim_count++;
                });
        },
        std::regex("sifive,itim0"), [&](node n) {
            auto name = n.name();
            n.named_tuples(
                "reg-names", "reg",
                "mem", tuple_t<target_addr, target_size>(), [&](target_addr base, target_size size) {
                    if (itim_count == 0)
                        dts_memory_list.push_back(memory("mem", "itim", "sifive,itim0", base, size));
                    itim_count++;
                });
        },
        std::regex("sifive,testram0"), [&](node n) {
            auto name = n.name();
            n.named_tuples(
                "reg-names", "reg",
                "mem", tuple_t<target_addr, target_size>(), [&](target_addr base, target_size size) {
                    if (testram_count == 0)
                        dts_memory_list.push_back(memory("mem", "testram", "sifive,testram0", base, size));
                    testram_count++;
                });
        },
        std::regex("sifive,ahb-periph-port"), [&](node n) {
            if (n.field_exists("ranges")) {
              target_addr address = get_memory_ranges_address(n);
              target_size size = get_memory_ranges_size(n);
                if (ahb_periph_count == 0)
                    dts_memory_list.push_back(memory("mem", "ahb_periph_ram", "sifive,ahb-periph-port", address, size));

                ahb_periph_count++;
            }
        },
        std::regex("sifive,apb-periph-port"), [&](node n) {
            if (n.field_exists("ranges")) {
              target_addr address = get_memory_ranges_address(n);
              target_size size = get_memory_ranges_size(n);
                if (apb_periph_count == 0)
                    dts_memory_list.push_back(memory("mem", "apb_periph_ram", "sifive,apb-periph-port", address, size));

                apb_periph_count++;
            }
        },
        std::regex("sifive,axi4-periph-port"), [&](node n) {
            if (n.field_exists("ranges")) {
              target_addr address = get_memory_ranges_address(n);
              target_size size = get_memory_ranges_size(n);
                if (axi4_periph_count == 0)
                    dts_memory_list.push_back(memory("mem", "axi4_periph_ram", "sifive,axi4-periph-port", address, size));

                axi4_periph_count++;
            }
        },
        std::regex("sifive,tl-periph-port"), [&](node n) {
            if (n.field_exists("ranges")) {
              target_addr address = get_memory_ranges_address(n);
              target_size size = get_memory_ranges_size(n);
                if (tl_periph_count == 0)
                    dts_memory_list.push_back(memory("mem", "tl_periph_ram", "sifive,tl-periph-port", address, size));

                tl_periph_count++;
            }
        },
        std::regex("sifive,ahb-sys-port"), [&](node n) {
            if (n.field_exists("ranges")) {
              target_addr address = get_memory_ranges_address(n);
              target_size size = get_memory_ranges_size(n);
                if (ahb_sys_count == 0)
                    dts_memory_list.push_back(memory("mem", "ahb_sys_ram", "sifive,ahb-sys-port", address, size));

                ahb_sys_count++;
            }
        },
        std::regex("sifive,apb-sys-port"), [&](node n) {
            if (n.field_exists("ranges")) {
              target_addr address = get_memory_ranges_address(n);
              target_size size = get_memory_ranges_size(n);
                if (apb_sys_count == 0)
                    dts_memory_list.push_back(memory("mem", "apb_sys_ram", "sifive,apb-sys-port", address, size));

                apb_sys_count++;
            }
        },
        std::regex("sifive,axi4-sys-port"), [&](node n) {
            if (n.field_exists("ranges")) {
              target_addr address = get_memory_ranges_address(n);
              target_size size = get_memory_ranges_size(n);
                if (axi4_sys_count == 0)
                    dts_memory_list.push_back(memory("mem", "axi4_sys_ram", "sifive,axi4-sys-port", address, size));

                axi4_sys_count++;
            }
        },
        std::regex("sifive,tl-sys-port"), [&](node n) {
            if (n.field_exists("ranges")) {
              target_addr address = get_memory_ranges_address(n);
              target_size size = get_memory_ranges_size(n);
                if (tl_sys_count == 0)
                    dts_memory_list.push_back(memory("mem", "tl_sys_ram", "sifive,tl-sys-port", address, size));

                tl_sys_count++;
            }
        },
        std::regex("sifive,sram0"), [&](node n) {
            auto name = n.name();
            n.named_tuples(
                "reg-names", "reg",
                "mem", tuple_t<target_addr, target_size>(), [&](target_addr base, target_size size) {
		  dts_memory_list.push_back(memory("mem", "sram" + std::to_string(sram_count),
						 "sifive,sram0", base, size));
                  sram_count++;
                });
        },
        std::regex("sifive,spi0"), [&](node n) {
            auto name = n.name();
            n.named_tuples(
                "reg-names", "reg",
                "control", tuple_t<target_addr, target_size>(), [&](target_addr base, target_size size) {},
                "mem", tuple_t<target_addr, target_size>(), [&](target_addr base, target_size size) {
                    if (spi_count == 0)
                        dts_memory_list.push_back(memory("mem", "spi", "sifive,spi0", base, size));
                    spi_count++;
                });
        });

    if (ahb_periph_count > 0) {
        if (memory_count > 0) {
            alias_memory("memory", "ram");
            alias_memory("ahb_periph_ram", "flash");
        } else if (sram_count > 1) {
            alias_memory("sram0", "ram");
            alias_memory("sram1", "itim");
            alias_memory("ahb_periph_ram", "flash");
        } else if (dtim_count > 0) {
            alias_memory("dtim", "ram");
            alias_memory("ahb_periph_ram", "flash");
        } else if (dcache_size > 0) {
            alias_memory("memory", "ram");
            alias_memory("ahb_periph_ram", "flash");
        } else {
            alias_memory("ahb_periph_ram", "ram");
        }
    } else if (apb_periph_count > 0) {
        if (memory_count > 0) {
            alias_memory("memory", "ram");
            alias_memory("apb_periph_ram", "flash");
        } else if (sram_count > 1) {
            alias_memory("sram0", "ram");
            alias_memory("sram1", "itim");
            alias_memory("apb_periph_ram", "flash");
        } else if (dtim_count > 0) {
            alias_memory("dtim", "ram");
            alias_memory("apb_periph_ram", "flash");
        } else if (dcache_size > 0) {
            alias_memory("memory", "ram");
            alias_memory("apb_periph_ram", "flash");
        } else {
            alias_memory("apb_periph_ram", "ram");
        }
    } else if (axi4_periph_count > 0) {
        if (memory_count > 0) {
            alias_memory("memory", "ram");
            alias_memory("axi4_periph_ram", "flash");
        } else if (sram_count > 1) {
            alias_memory("sram0", "ram");
            alias_memory("sram1", "itim");
            alias_memory("axi4_periph_ram", "flash");
        } else if (dtim_count > 0) {
            alias_memory("dtim", "ram");
            alias_memory("axi4_periph_ram", "flash");
        } else if (dcache_size > 0) {
            alias_memory("memory", "ram");
            alias_memory("axi4_periph_ram", "flash");
        } else {
            alias_memory("axi4_periph_ram", "ram");
        }
    } else if (tl_periph_count > 0) {
        if (memory_count > 0) {
            alias_memory("memory", "ram");
            alias_memory("tl_periph_ram", "flash");
        } else if (sram_count > 1) {
            alias_memory("sram0", "ram");
            alias_memory("sram1", "itim");
            alias_memory("tl_periph_ram", "flash");
        } else if (dtim_count > 0) {
            alias_memory("dtim", "ram");
            alias_memory("tl_periph_ram", "flash");
        } else if (dcache_size > 0) {
            alias_memory("memory", "ram");
            alias_memory("tl_periph_ram", "flash");
        } else {
            alias_memory("tl_periph_ram", "ram");
        }
    } else if (ahb_sys_count > 0) {
        if (memory_count > 0) {
            alias_memory("memory", "ram");
            alias_memory("ahb_sys_ram", "flash");
        } else if (sram_count > 1) {
            alias_memory("sram0", "ram");
            alias_memory("sram1", "itim");
            alias_memory("ahb_sys_ram", "flash");
        } else if (dtim_count > 0) {
            alias_memory("dtim", "ram");
            alias_memory("ahb_sys_ram", "flash");
        } else if (dcache_size > 0) {
            alias_memory("memory", "ram");
            alias_memory("ahb_sys_ram", "flash");
        } else {
            alias_memory("ahb_sys_ram", "ram");
        }
    } else if (apb_sys_count > 0) {
        if (memory_count > 0) {
            alias_memory("memory", "ram");
            alias_memory("apb_sys_ram", "flash");
        } else if (sram_count > 1) {
            alias_memory("sram0", "ram");
            alias_memory("sram1", "itim");
            alias_memory("apb_sys_ram", "flash");
        } else if (dtim_count > 0) {
            alias_memory("dtim", "ram");
            alias_memory("apb_sys_ram", "flash");
        } else if (dcache_size > 0) {
            alias_memory("memory", "ram");
            alias_memory("apb_sys_ram", "flash");
        } else {
            alias_memory("apb_sys_ram", "ram");
        }
    } else if (axi4_sys_count > 0) {
        if (memory_count > 0) {
            alias_memory("memory", "ram");
            alias_memory("axi4_sys_ram", "flash");
        } else if (sram_count > 1) {
            alias_memory("sram0", "ram");
            alias_memory("sram1", "itim");
            alias_memory("axi4_sys_ram", "flash");
        } else if (dtim_count > 0) {
            alias_memory("dtim", "ram");
            alias_memory("axi4_sys_ram", "flash");
        } else if (dcache_size > 0) {
            alias_memory("memory", "ram");
            alias_memory("axi4_sys_ram", "flash");
        } else {
            alias_memory("axi4_sys_ram", "ram");
        }
    } else if (tl_sys_count > 0) {
        if (memory_count > 0) {
            alias_memory("memory", "ram");
            alias_memory("tl_sys_ram", "flash");
        } else if (sram_count > 1) {
            alias_memory("sram0", "ram");
            alias_memory("sram1", "itim");
            alias_memory("tl_sys_ram", "flash");
        } else if (dtim_count > 0) {
            alias_memory("dtim", "ram");
            alias_memory("tl_sys_ram", "flash");
        } else if (dcache_size > 0) {
            alias_memory("memory", "ram");
            alias_memory("tl_sys_ram", "flash");
        } else {
            alias_memory("tl_sys_ram", "ram");
        }
    } else if (testram_count > 0) {
        if (memory_count > 0) {
            alias_memory("memory", "ram");
            alias_memory("testram", "flash");
        } else if (sram_count > 1) {
            alias_memory("sram0", "ram");
            alias_memory("sram1", "itim");
	    alias_memory("testram", "flash");
        } else if (dtim_count > 0) {
            alias_memory("dtim", "ram");
            alias_memory("testram", "flash");
        } else {
            alias_memory("testram", "ram");
        }
    } else if (memory_count > 0) {
        alias_memory("memory", "ram");
    	if (spi_count > 0) {
            alias_memory("spi", "flash");
        }
    } else if (sram_count > 0) {
        if (sram_count == 1 && ramrodata) {
            split_sram("sram0");
        }
        alias_memory("sram0", "ram");
        alias_memory("sram1", "itim");
        alias_memory("spi", "flash");
    } else {
        alias_memory("dtim", "ram");
        alias_memory("spi", "flash");
    }
}

static void show_dts_memory (string mtype)
{
    std::cout << "#" << __FUNCTION__ << std::endl;
    std::cout << std::endl << std::endl;
    std::cout << "MEMORY" << std::endl << "{" << std::endl;
    for (auto entry : dts_memory_list) {
      if ((entry.mem_type.compare(mtype) == 0) || (mtype.compare("all") == 0)) {
	std::cout << "\t" << entry.mem_alias << "/" << entry.mem_name;
	std::cout << ((entry.mem_type.compare("control") == 0) ? " (rw)" : " (rwx)");
	std::cout << " : ORIGIN = 0x" << std::hex << entry.mem_start;
	std::cout << ", LENGTH = 0x" << entry.mem_length;
	std::cout << std::dec << std::endl;
      }
    }
    std::cout << "}" << std::endl;
}

static void write_linker_file (fstream &os, std::string release)
{
    write_banner(os, release);
    os << "OUTPUT_ARCH(\"riscv\")" << std::endl << std::endl;
    os << "ENTRY(_enter)" << std::endl << std::endl;
}

static void write_linker_memory (fstream &os, bool scratchpad, uint32_t metal_entry)
{
  int flash_offset = 0;
    os << "MEMORY" << std::endl << "{" << std::endl;
    for (auto entry : dts_memory_list) {
      if ((entry.mem_type.compare("mem") == 0) &&
        (entry.mem_alias.compare("flash") == 0)) {
        if (entry.mem_name.find("spi") != std::string::npos) {
          flash_offset = metal_entry;
          entry.mem_length -= metal_entry;
        }
        os << "\t" << entry.mem_alias <<  " (rxai!w)";
      } else if (entry.mem_alias.compare("ram") == 0) {
        os << "\t" << entry.mem_alias <<  " (wxa!ri)";
      } else if (entry.mem_alias.compare("itim") == 0) {
        os << "\t" << entry.mem_alias <<  " (wx!rai)";
        flash_offset = 0;
      } else {
        continue;
      }
      os << " : ORIGIN = 0x" << std::hex << (entry.mem_start + flash_offset);
      /* FIXME: Here we restrict the length of any segment to 2GiB.  While this
       * isn't technically correct, it does at least prevent us from ending up
       * with segments that are too large for */
      os << ", LENGTH = 0x" << ((entry.mem_length > 0x80000000UL) ? 0x80000000UL : entry.mem_length);
      os << std::dec << std::endl;
    }
    os << "}" << std::endl << std::endl;
}

static void write_linker_phdrs (fstream &os, bool scratchpad)
{
    os << "PHDRS" << std::endl << "{" << std::endl;
    os << "\tflash PT_LOAD;" << std::endl;
    os << "\tram_init PT_LOAD;" << std::endl;
    os << "\titim_init PT_LOAD;" << std::endl;
    if (scratchpad == false) {
        os << "\tram PT_NULL;" << std::endl;
        os << "\titim PT_NULL;" << std::endl;
    } else {
        os << "\tram PT_LOAD;" << std::endl;
        os << "\titim PT_LOAD;" << std::endl;
    }
    os << "}" << std::endl << std::endl;
}

static void write_linker_sections (fstream &os, int num_harts, int boot_hart, bool scratchpad, bool ramrodata, bool itim, int chicken_bit)
{
    std::string stack_cfg = "0x400";
    std::string heap_cfg = "0x400";

    os << "SECTIONS" << std::endl << "{" << std::endl;

    /* Define stack size */
    os << "\t__stack_size = DEFINED(__stack_size) ? __stack_size : "
	      << stack_cfg << ";" << std::endl;
    os << "\tPROVIDE(__stack_size = __stack_size);" << std::endl;
    /* Define heap size */
    os << "\t__heap_size = DEFINED(__heap_size) ? __heap_size : "
	      << heap_cfg << ";" << std::endl;
    os << "\tPROVIDE(__metal_boot_hart = " << std::to_string(boot_hart) << ");" << std::endl;
    os << "\tPROVIDE(__metal_chicken_bit = " << std::to_string(chicken_bit) << ");" << std::endl;

    os << std::endl << std::endl;
    /* Define init section */
    os << "\t.init \t\t:" << std::endl;
    os << "\t{" << std::endl;
    os << "\t\tKEEP (*(.text.metal.init.enter))" << std::endl;
    os << "\t\tKEEP (*(.text.metal.init.*))" << std::endl;
    os << "\t\tKEEP (*(SORT_NONE(.init)))" << std::endl;
    os << "\t\tKEEP (*(.text.libgloss.start))" << std::endl;
    if (scratchpad) {
      os << "\t} >ram AT>ram :ram" << std::endl;
    } else {
      os << "\t} >flash AT>flash :flash" << std::endl;
    }

    os << std::endl << std::endl;
    if (!ramrodata) {
      /* Define text section */
      os << "\t.text \t\t:" << std::endl;
      os << "\t{" << std::endl;
      os << "\t\t*(.text.unlikely .text.unlikely.*)" << std::endl;
      os << "\t\t*(.text.startup .text.startup.*)" << std::endl;
      os << "\t\t*(.text .text.*)" << std::endl;
      if (!itim && !has_memory("itim")) {
	os << "\t\t*(.itim .itim.*)" << std::endl;
      }
      os << "\t\t*(.gnu.linkonce.t.*)" << std::endl;
      if (scratchpad) {
	os << "\t} >ram AT>ram :ram" << std::endl;
      } else {
	os << "\t} >flash AT>flash :flash" << std::endl;
      }
    }

    os << std::endl << std::endl;
    /* Define fini section */
    os << "\t.fini \t\t:" << std::endl;
    os << "\t{" << std::endl;
    os << "\t\tKEEP (*(SORT_NONE(.fini)))" << std::endl;
    if (scratchpad) {
      os << "\t} >ram AT>ram :ram" << std::endl;
    } else {
      os << "\t} >flash AT>flash :flash" << std::endl;
    }

    os << std::endl << std::endl;
    /* Define etext labels */
    os << "\tPROVIDE (__etext = .);" << std::endl;
    os << "\tPROVIDE (_etext = .);" << std::endl;
    os << "\tPROVIDE (etext = .);" << std::endl;

    os << std::endl << std::endl;
    /* Define rodata section */
    if (!ramrodata) {
      os << "\t.rodata \t\t:" << std::endl;
      os << "\t{" << std::endl;
      os << "\t\t*(.rdata)" << std::endl;
      os << "\t\t*(.rodata .rodata.*)" << std::endl;
      os << "\t\t*(.gnu.linkonce.r.*)" << std::endl;
      os << "\t\t. = ALIGN(8);" << std::endl;
      os << "\t\t*(.srodata.cst16)" << std::endl;
      os << "\t\t*(.srodata.cst8)" << std::endl;
      os << "\t\t*(.srodata.cst4)" << std::endl;
      os << "\t\t*(.srodata.cst2)" << std::endl;
      os << "\t\t*(.srodata .srodata.*)" << std::endl;
      if (scratchpad) {
	os << "\t} >ram AT>ram :ram" << std::endl;
      } else {
	os << "\t} >flash AT>flash :flash" << std::endl;
      }
    }

    os << std::endl << std::endl;
    os << "\t. = ALIGN(8);" << std::endl;

    os << std::endl << std::endl;
    /* Define preinit_array section */
    os << "\t.preinit_array \t\t:" << std::endl;
    os << "\t{" << std::endl;
    os << "\t\tPROVIDE_HIDDEN (__preinit_array_start = .);" << std::endl;
    os << "\t\tKEEP (*(.preinit_array))" << std::endl;
    os << "\t\tPROVIDE_HIDDEN (__preinit_array_end = .);" << std::endl;
    if (scratchpad) {
      os << "\t} >ram AT>ram :ram" << std::endl;
    } else {
      os << "\t} >flash AT>flash :flash" << std::endl;
    }

    os << std::endl << std::endl;
    /* Define init_array section */
    os << "\t.init_array \t\t:" << std::endl;
    os << "\t{" << std::endl;
    os << "\t\tPROVIDE_HIDDEN (__init_array_start = .);" << std::endl;
    os << "\t\tKEEP (*(SORT_BY_INIT_PRIORITY(.init_array.*) SORT_BY_INIT_PRIORITY(.ctors.*)))" << std::endl;
    os << "\t\tKEEP (*(.init_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .ctors))" << std::endl;
    os << "\t\tPROVIDE_HIDDEN (__init_array_end = .);" << std::endl;
    if (scratchpad) {
      os << "\t} >ram AT>ram :ram" << std::endl;
    } else {
      os << "\t} >flash AT>flash :flash" << std::endl;
    }

    os << std::endl << std::endl;
    /* Define fini_array section */
    os << "\t.fini_array \t\t:" << std::endl;
    os << "\t{" << std::endl;
    os << "\t\tPROVIDE_HIDDEN (__fini_array_start = .);" << std::endl;
    os << "\t\tKEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))" << std::endl;
    os << "\t\tKEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))" << std::endl;
    os << "\t\tPROVIDE_HIDDEN (__fini_array_end = .);" << std::endl;
    if (scratchpad) {
      os << "\t} >ram AT>ram :ram" << std::endl;
    } else {
      os << "\t} >flash AT>flash :flash" << std::endl;
    }

    os << std::endl << std::endl;
    /* Define ctors section */
    os << "\t.ctors \t\t:" << std::endl;
    os << "\t{" << std::endl;
    os << "\t\t/* gcc uses crtbegin.o to find the start of" << std::endl;
    os << "\t\t   the constructors, so we make sure it is" << std::endl;
    os << "\t\t   first.  Because this is a wildcard, it" << std::endl;
    os << "\t\t   doesn't matter if the user does not" << std::endl;
    os << "\t\t   actually link against crtbegin.o; the" << std::endl;
    os << "\t\t   linker won't look for a file to match a" << std::endl;
    os << "\t\t   wildcard.  The wildcard also means that it" << std::endl;
    os << "\t\t   doesn't matter which directory crtbegin.o" << std::endl;
    os << "\t\t   is in.  */" << std::endl;
    os << "\t\tKEEP (*crtbegin.o(.ctors))" << std::endl;
    os << "\t\tKEEP (*crtbegin?.o(.ctors))" << std::endl;
    os << "\t\t/* We don't want to include the .ctor section from" << std::endl;
    os << "\t\t   the crtend.o file until after the sorted ctors." << std::endl;
    os << "\t\t   The .ctor section from the crtend file contains the" << std::endl;
    os << "\t\t   end of ctors marker and it must be last */" << std::endl;
    os << "\t\tKEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .ctors))" << std::endl;
    os << "\t\tKEEP (*(SORT(.ctors.*)))" << std::endl;
    os << "\t\tKEEP (*(.ctors))" << std::endl;
    if (scratchpad) {
      os << "\t} >ram AT>ram :ram" << std::endl;
    } else {
      os << "\t} >flash AT>flash :flash" << std::endl;
    }

    os << std::endl << std::endl;
    /* Define dtors section */
    os << "\t.dtors \t\t:" << std::endl;
    os << "\t{" << std::endl;
    os << "\t\tKEEP (*crtbegin.o(.dtors))" << std::endl;
    os << "\t\tKEEP (*crtbegin?.o(.dtors))" << std::endl;
    os << "\t\tKEEP (*(EXCLUDE_FILE (*crtend.o *crtend?.o ) .dtors))" << std::endl;
    os << "\t\tKEEP (*(SORT(.dtors.*)))" << std::endl;
    os << "\t\tKEEP (*(.dtors))" << std::endl;
    if (scratchpad) {
      os << "\t} >ram AT>ram :ram" << std::endl;
    } else {
      os << "\t} >flash AT>flash :flash" << std::endl;
    }

    os << std::endl << std::endl;
    /* Define litimalign section */
    os << "\t.litimalign \t\t:" << std::endl;
    os << "\t{" << std::endl;
    os << "\t\t. = ALIGN(8);" << std::endl;
    os << "\t\tPROVIDE( metal_segment_itim_source_start = . );" << std::endl;
    if (scratchpad) {
      os << "\t} >ram AT>ram :ram" << std::endl;
    } else {
      os << "\t} >flash AT>flash :flash" << std::endl;
    }

    os << std::endl << std::endl;
    /* Define ditimalign section */
    os << "\t.ditimalign \t\t:" << std::endl;
    os << "\t{" << std::endl;
    os << "\t\t. = ALIGN(8);" << std::endl;
    os << "\t\tPROVIDE( metal_segment_itim_target_start = . );" << std::endl;
    if (itim || has_memory("itim")) {
      if (scratchpad) {
        os << "\t} >itim AT>ram :itim_init" << std::endl;
      } else {
        os << "\t} >itim AT>flash :itim_init" << std::endl;
      }
    } else {
      if (scratchpad) {
        os << "\t} >ram AT>ram :ram_init" << std::endl;
      } else {
        os << "\t} >ram AT>flash :ram_init" << std::endl;
      }
    }

    os << std::endl << std::endl;
    /* Define itim section */
    os << "\t.itim \t\t:" << std::endl;
    os << "\t{" << std::endl;
    os << "\t\t*(.itim .itim.*)" << std::endl;
    if (ramrodata && itim) {
      os << "\t\t*(.text.unlikely .text.unlikely.*)" << std::endl;
      os << "\t\t*(.text.startup .text.startup.*)" << std::endl;
      os << "\t\t*(.text .text.*)" << std::endl;
      os << "\t\t*(.gnu.linkonce.t.*)" << std::endl;
    }
    if (itim || has_memory("itim")) {
      if (scratchpad) {
        os << "\t} >itim AT>ram :itim_init" << std::endl;
      } else {
        os << "\t} >itim AT>flash :itim_init" << std::endl;
      }
    } else {
      if (scratchpad) {
        os << "\t} >ram AT>ram :ram_init" << std::endl;
      } else {
        os << "\t} >flash AT>flash :flash" << std::endl;
      }
    }

    os << std::endl << std::endl;
    /* Define end labels */
    os << "\tPROVIDE( metal_segment_itim_target_end = . );" << std::endl;

    if (ramrodata) {
      /* Define text section */
      os << "\t.text \t\t:" << std::endl;
      os << "\t{" << std::endl;
      os << "\t\t*(.text.unlikely .text.unlikely.*)" << std::endl;
      os << "\t\t*(.text.startup .text.startup.*)" << std::endl;
      os << "\t\t*(.text .text.*)" << std::endl;
      os << "\t\t*(.gnu.linkonce.t.*)" << std::endl;
      if (!itim) {
        if (!has_memory("itim")) {
          os << "\t\t*(.itim .itim.*)" << std::endl;
	}
        if (scratchpad) {
          os << "\t} >ram AT>ram :ram" << std::endl;
        } else {
          os << "\t} >flash AT>flash :flash" << std::endl;
        }
      } else {
        os << "\t} >itim AT>flash :flash" << std::endl;
      }
    }

    os << std::endl << std::endl;
    /* Define lalign section */
    os << "\t.lalign \t\t:" << std::endl;
    os << "\t{" << std::endl;
    os << "\t\t. = ALIGN(8);" << std::endl;
    os << "\t\tPROVIDE( _data_lma = . );" << std::endl;
    os << "\t\tPROVIDE( metal_segment_data_source_start = . );" << std::endl;
    if (scratchpad) {
      os << "\t} >ram AT>ram :ram" << std::endl;
    } else {
      os << "\t} >flash AT>flash :flash" << std::endl;
    }

    os << std::endl << std::endl;
    /* Define dalign section */
    os << "\t.dalign \t\t:" << std::endl;
    os << "\t{" << std::endl;
    os << "\t\t. = ALIGN(8);" << std::endl;
    os << "\t\tPROVIDE( metal_segment_data_target_start = . );" << std::endl;
    if (scratchpad) {
      os << "\t} >ram AT>ram :ram_init" << std::endl;
    } else {
      os << "\t} >ram AT>flash :ram_init" << std::endl;
    }

    os << std::endl << std::endl;
    /* Define data section */
    os << "\t.data \t\t:" << std::endl;
    os << "\t{" << std::endl;
    if (ramrodata) {
      os << "\t\t*(.rdata)" << std::endl;
      os << "\t\t*(.rodata .rodata.*)" << std::endl;
      os << "\t\t*(.gnu.linkonce.r.*)" << std::endl;
      os << "\t\t. = ALIGN(8);" << std::endl;
      os << "\t\t*(.srodata.cst16)" << std::endl;
      os << "\t\t*(.srodata.cst8)" << std::endl;
      os << "\t\t*(.srodata.cst4)" << std::endl;
      os << "\t\t*(.srodata.cst2)" << std::endl;
      os << "\t\t*(.srodata .srodata.*)" << std::endl;
    }
    os << "\t\t*(.data .data.*)" << std::endl;
    os << "\t\t*(.gnu.linkonce.d.*)" << std::endl;
    os << "\t\t. = ALIGN(8);" << std::endl;
    os << "\t\tPROVIDE( __global_pointer$ = . + 0x800 );" << std::endl;
    os << "\t\t*(.sdata .sdata.* .sdata2.*)" << std::endl;
    os << "\t\t*(.gnu.linkonce.s.*)" << std::endl;
    if (scratchpad) {
      os << "\t} >ram AT>ram :ram_init" << std::endl;
    } else {
      os << "\t} >ram AT>flash :ram_init" << std::endl;
    }

    os << std::endl << std::endl;
    /* Define some more end.. labels */
    os << "\tPROVIDE( _edata = . );" << std::endl;
    os << "\tPROVIDE( edata = . );" << std::endl;
    os << "\tPROVIDE( metal_segment_data_target_end = . );" << std::endl;

    os << "\tPROVIDE( _fbss = . );" << std::endl;
    os << "\tPROVIDE( __bss_start = . );" << std::endl;
    os << "\tPROVIDE( metal_segment_bss_target_start = . );" << std::endl;

    os << std::endl << std::endl;
    /* Define bss section */
    os << "\t.bss \t\t:" << std::endl;
    os << "\t{" << std::endl;
    os << "\t\t*(.sbss*)" << std::endl;
    os << "\t\t*(.gnu.linkonce.sb.*)" << std::endl;
    os << "\t\t*(.bss .bss.*)" << std::endl;
    os << "\t\t*(.gnu.linkonce.b.*)" << std::endl;
    os << "\t\t*(COMMON)" << std::endl;
    os << "\t} >ram AT>ram :ram" << std::endl;

    os << std::endl << std::endl;
    /* Define end labels */
    os << "\tPROVIDE( _end = . );" << std::endl;
    os << "\tPROVIDE( end = . );" << std::endl;
    os << "\tPROVIDE( metal_segment_bss_target_end = . );" << std::endl;
    os << std::endl << std::endl;

    /* Define stack section */
    os << "\t.stack :" << std::endl;
    os << "\t{" << std::endl;
    os << "\t\tPROVIDE(metal_segment_stack_begin = .);" << std::endl;
    os << "\t\t. = __stack_size;" << std::endl;
    os << "\t\tPROVIDE( _sp = . );" << std::endl;
    for (int i = 1; i < num_harts; i++) {
      os << "\t\t. = __stack_size;" << std::endl;
    }
    os << "\t\tPROVIDE(metal_segment_stack_end = .);" << std::endl;
    os << "\t} >ram AT>ram :ram" << std::endl;

    os << std::endl << std::endl;

    os << "\t.heap :" << std::endl;
    os << "\t{" << std::endl;
    os << "\t\tPROVIDE( metal_segment_heap_target_start = . );" << std::endl;

    os << "\t\t. = __heap_size;" << std::endl;

    os << "\t\tPROVIDE( metal_segment_heap_target_end = . );" << std::endl;;
    os << "\t\tPROVIDE( _heap_end = . );" << std::endl;
    os << "\t} >ram AT>ram :ram" << std::endl;

    os << std::endl << std::endl;
    os << "}" << std::endl << std::endl;
}

static void show_usage(string name)
{
  std::cerr << "Usage: " << name << " <option(s)>\n"
	    << "General Options:\n"
	    << "    -h,--help                               Show this help message\n"
        << "Input/Output Options:\n"
	    << "    -d,--dtb <eg. xxx.dtb>                  Specify fullpath to the DTB file\n"
	    << "    -l,--linker <eg. zzz.lds>               Generate linker file to fullpath filename\n"
        << "Linker Script Generation Options:\n"
	    << "    --scratchpad                            Execute codes from ram, -l option\n"
	    << "    --ramrodata                             Rodata in ram data section, -l option\n"
	    << "    -s,--show <eg. mem | control| both>     Show all types\n"
	    << endl;
}

int main (int argc, char* argv[])
{
  string show;
  string dtb_file;
  string linker_file;
  string release;
  bool ramrodata = false;
  bool scratchpad = false;
  bool itim = false;

  if ((argc < 2) && (argc > 7)) {
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
          } else if ((arg == "-l") || (arg == "--linker")) {
              if (i + 1 < argc) {
                  linker_file = argv[++i];
              } else {
                  std::cerr << "--linker option requires file path." << std::endl;
                  show_usage(argv[0]);
                  return 1;
              }
          } else if (arg == "--ramrodata") {
	      if (!linker_file.empty()) {
		ramrodata = true;
              } else {
                  std::cerr << "--linker option requires." << std::endl;
                  return 1;
              }
          } else if (arg == "--scratchpad") {
	      if (!linker_file.empty()) {
		scratchpad = true;
              } else {
                  std::cerr << "--linker option requires." << std::endl;
                  return 1;
              }
          } else if ((arg == "-s") || (arg == "--show")) {
              if (i + 1 < argc) {
                  show = argv[++i];
		  if ((show.compare("mem") != 0) &&
		      (show.compare("control") != 0) &&
		      (show.compare("all") != 0)) {
		    std::cerr << "Possible options are <mem | control | all>."
			      << std::endl;
		    return 1;
		  }
              } else {
		  show = "all";
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
  dts_blob = dts_read(dtb_file.c_str());
  if (!dts_blob) {
      std::cerr << "ABORT: Failed to read " << dtb_file << std::endl;
      return 1;
  }

  get_dts_attribute("/cpus/cpu@0", "riscv,isa");
  dts_memory(ramrodata);

  /**
   * scratchpad is when we load and run everything from memory, whether it is
   * dtim, testram.
   */
  if ( !linker_has_memory("flash") ) {
    scratchpad = true;
  }

  /*
   * Detect if we posess an ITIM and it can fit
   */
  itim = linker_has_memory("itim");

  if ( !linker_file.empty() ) {
    std::fstream lds;

    lds.open(linker_file, fstream::in | fstream::out | fstream::trunc);
    if (lds.is_open() == false) {
      std::cerr << "Error: Failed to create " << linker_file << std::endl;
      return 1;
    }

    /* Get the value of the metal,entry chosen node */
    uint32_t metal_entry = 0;
    auto dtb = fdt((const uint8_t *)dts_blob);
    dtb.chosen(
      "metal,entry",
      tuple_t<node, uint32_t>(),
      [&](node n, uint32_t offset) {
        metal_entry = offset;
      });

    int num_harts = 0;
    dtb.match(
      std::regex("cpu"),
      [&](node n) {
        num_harts += 1;
      });

    int boot_hart = 0;
    dtb.chosen(
      "metal,boothart",
      tuple_t<node>(),
      [&](node n) {
        boot_hart = std::stoi(n.instance());
      });

    std::cout << "Found " << num_harts << " harts\n";

    int chicken_bit = 0;
    string cpucompat;
    string boothart_str;
    boothart_str = "/cpus/cpu@" + std::to_string(boot_hart);
    cpucompat = get_dts_attribute(boothart_str, "compatible");
    if(cpucompat.find("bullet") != std::string::npos) {
        chicken_bit = 1;
    }

    write_linker_file(lds, release);
    write_linker_memory(lds, scratchpad, metal_entry);
    write_linker_phdrs(lds, scratchpad);
    write_linker_sections(lds, num_harts, boot_hart, scratchpad, ramrodata, itim, chicken_bit);
  }

  if (!show.empty()) {
    show_dts_memory(show);
  }

  return 0;
}
