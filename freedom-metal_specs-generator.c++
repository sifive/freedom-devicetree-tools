/* Copyright 2018 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <stdlib.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdarg.h>
#include <iostream>
#include <fstream>
#include <string>
#include <sstream>
#include <vector>
#include <iterator>
#ifdef __cplusplus
extern "C"{
#endif
#include <libfdt.h>
#ifdef __cplusplus
}
#endif
#include "multilib.h++"
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
  int mem_base;
  int mem_start;
  int mem_length;

  memory() {}
  memory(std::string, std::string, std::string, int, int, int);
};

memory::memory (std::string mtype, std::string alias, std::string name,
		int base, int start, int length)
{
  mem_type = mtype;
  mem_alias = alias;
  mem_name = name;
  mem_base = base;
  mem_start = start;
  mem_length = length;
}

static char *dts_blob;
static std::vector<memory> dts_memory_list;

static bool find_memory (std::string mem, std::string mtype, memory &object)
{
  for (auto entry : dts_memory_list) {
    if ((entry.mem_name.compare(mem) == 0)
	&&
	(entry.mem_type.compare(mtype) == 0)) {
      std::cout << "found " << mem << std::endl;
      object = entry;
      return true;
    }
  }
  return false;
}

static bool has_memory (std::string mem)
{
  for (auto entry : dts_memory_list) {
    if (entry.mem_alias.compare(mem) == 0) {
      std::cout << "found " << mem << std::endl;
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
      break;
    }
  }
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

static string get_dts_attribute (const string path, const string tag)
{
    int node, len;
    const char *value;

    node = fdt_path_offset(dts_blob, path.c_str());
    if (node < 0) return "";
    value = (const char *)fdt_getprop(dts_blob, node, tag.c_str(), &len);
    std::cout << string(value) << std::endl;
    return string(value);
}

static void dts_memory (void)
{
    int offset, depth = 0;

    if (!dts_blob)
        return;

    std::cout << __FUNCTION__ << std::endl;

    offset = fdt_path_offset(dts_blob, "/soc");
    for (offset = fdt_next_node(dts_blob, offset, &depth);
	 offset >= 0 && depth >= 0;
	 offset = fdt_next_node(dts_blob, offset, &depth)) {
        const char *nodep;
        const char *regnamep, *s;
	int regnamelen;

        nodep = fdt_get_name(dts_blob, offset, NULL);
	regnamep = (const char *)fdt_getprop(dts_blob, offset, "reg-names", &regnamelen);
	if (regnamelen > 0) {
	    const char *regp;
	    int reglen, regidx;
	    s = regnamep;
	    regidx = 0;
	    do {
	        long base = 0;
		std::vector<std::string> splices = split(string(nodep), '@');
		regp = (const char *)fdt_getprop(dts_blob, offset, "reg", &reglen);
		if (reglen > 0) {
		    const fdt32_t *cell = (const fdt32_t *)regp;
		    base = std::stol(splices[1], nullptr, 16);
		    dts_memory_list.push_back(memory(s, splices[0], splices[0],
						     base,
						     fdt32_to_cpu(cell[regidx]),
						     fdt32_to_cpu(cell[regidx + 1]))
					      );
		}
		s += strlen(regnamep) + 1;
		regidx += 2;
	    } while (s < regnamep + regnamelen);
	}
    }
    alias_memory("dtim", "ram");
    alias_memory("spi", "flash");
}

static void show_dts_attributes (void)
{
}

static void write_specs_file (fstream &os, std::string machine, std::string prefix, std::string tuple, std::string gcc_version)
{
    string isa = arch2arch(get_dts_attribute("/cpus/cpu@0", "riscv,isa"));
    string abi = arch2abi(isa);
    string emul = arch2elf(isa);

    os << "%rename asm  metal_machine__asm\n";
    os << "*asm:\n";
    os << "-march=" << isa << " -mabi=" << abi << " %(metal_machine__asm)\n";
    os << "\n";

    os << "%rename cpp  metal_machine__cpp\n";
    os << "*cpp:\n";
    os << "-D__METAL_MACHINE_HEADER=\"metal/machine/" << machine << ".h\" %(metal_machine__cpp)\n";
    os << "\n";

    os << "%rename cc1  metal_machine__cc1\n";
    os << "*cc1:\n";
    os << "-ffunction-sections -fdata-sections -march=" << isa << " -mabi=" << abi << " %(metal_machine__cc1)\n";
    os << "\n";

    os << "%rename cc1plus  metal_machine__cc1plus\n";
    os << "*cc1plus:\n";
    os << "-ffunction-sections -fdata-sections -march=" << isa << " -mabi=" << abi << " %(metal_machine__cc1plus)\n";
    os << "\n";

    os << "*startfile:\n";
    os << "%{!nostartfiles:"  << prefix << "/" << tuple << "/lib/" << isa << "/" << abi << "/crt0__metal.o"
       << " " << prefix << "/lib/gcc/" << tuple << "/" << gcc_version << "/" << isa << "/" << abi << "/crtbegin.o}\n";
    os << "\n";

    os << "*endfile:\n";
    os << "%{!nostartfiles:"  << prefix << "/lib/gcc/" << tuple << "/" << gcc_version << "/" << isa << "/" << abi << "/crtend.o}\n";
    os << "\n";

    os << "%rename link   metal_machine__link\n";
    os << "*link:\n";
    os << "%{!T*:-Triscv__mmachine__" << machine << ".lds}"
       << " --gc-sections"
       << " -march=" << isa
       << " -mabi=" << abi
       << " -m" << emul
       << " -L" << prefix << "/" << tuple << "/lib/" << isa << "/" << abi << "/"
       << " -L" << prefix << "/lib/gcc/" << tuple << "/" << gcc_version << "/" << isa << "/" << abi << "/"
       << " %{!nostdlib:-lgcc}"
       << " %(metal_machine__link)\n";
    os << "\n";
}

static void show_usage(string name)
{
  std::cerr << "Usage: " << name << " <option(s)>\n"
	    << "Options:\n"
	    << "\t-h,--help\t\t\tShow this help message\n"
	    << "\t-m,--machine <eg. xxx>\t\tSpecify machine name\n"
	    << "\t-d,--dtb <eg. xxx.dtb>\t\tSpecify fullpath to the DTB file\n"
	    << "\t-s,--specs <eg. xxx.specs>\t\tGenerate GCC specs file\n"
	    << "\t-p,--prefix <eg, $prefix>\t\tLook for the header files here\n"
	    << endl;
}

int main (int argc, char* argv[])
{
  bool show = false;
  string machine;
  string dtb_file;
  string specs_file;
  string prefix;

  if ((argc < 2) && (argc > 5)) {
      show_usage(argv[0]);
      return 1;
  } else {
      for (int i = 1; i < argc; ++i) {
          string arg = argv[i];
          if ((arg == "-m") || (arg == "--machine")) {
              if (i + 1 < argc) {
                  machine = argv[++i];
              }
          } else if ((arg == "-d") || (arg == "--dtb")) {
              if (i + 1 < argc) {
                  dtb_file = argv[++i];
              } else {
                  std::cerr << "--dtb option requires file path." << std::endl;
                  show_usage(argv[0]);
                  return 1;
              }
          } else if ((arg == "-s") || (arg == "--specs")) {
              if (i + 1 < argc) {
                  specs_file = argv[++i];
              } else {
                  std::cerr << "--specs option requires file path." << std::endl;
                  show_usage(argv[0]);
                  return 1;
              }
          } else if ((arg == "-p") || (arg == "--prefix")) {
              if (i + 1 < argc) {
                  prefix = argv[++i];
              } else {
                  std::cerr << "--prefix option requires directory path." << std::endl;
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
  dts_blob = dts_read(dtb_file.c_str());
  if (!dts_blob) {
      std::cerr << "ABORT: Failed to read " << dtb_file << std::endl;
      return 1;
  }

  if (prefix.empty()) {
    std::cerr << "--prefix option requires a directory path." << std::endl;
    show_usage(argv[0]);
    return 1;
  }

  dts_memory();

  if (!specs_file.empty()) {
    std::fstream cfg;

    cfg.open(specs_file, fstream::in | fstream::out | fstream::trunc);
    if (cfg.is_open() == false) {
      std::cerr << "Error: Failed to create " << specs_file << std::endl;
      return 1;
    }

    write_specs_file(cfg, machine, prefix, "riscv64-sifive-elf", "8.1.0");
  }

  if (show) {
    show_dts_attributes();
  }

  return 0;
}
