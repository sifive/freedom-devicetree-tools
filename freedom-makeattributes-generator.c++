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
    std::cout << "#" << __FUNCTION__ << std::endl;
    std::cout << std::endl << std::endl;
    get_dts_attribute("/cpus/cpu@0", "riscv,isa");
    get_dts_attribute("/soc/serial@20000000", "compatible");
}

static void write_config_file (fstream &os, std::string board)
{
    string isa;
    string serial;

    isa = arch2arch(get_dts_attribute("/cpus/cpu@0", "riscv,isa"));
    std::size_t found = isa.find("rv64");
    os << "RISCV_ARCH=" << isa << std::endl;
    os << "RISCV_ABI=" << arch2abi(isa) << std::endl;
    if (found != std::string::npos) {
        os << "RISCV_CMODEL=medany" << std::endl;
    } else {
        os << "RISCV_CMODEL=medlow" << std::endl;
    }
    os << std::endl;

    if (board.find("rtl") != std::string::npos) {
        if (found != std::string::npos) {
            os << "COREIP_MEM_WIDTH=64" << std::endl;
        } else {
            os << "COREIP_MEM_WIDTH=32" << std::endl;
        }
        os << std::endl;
        os << "TARGET_TAGS=rtl" << std::endl;
    } else if (board.find("arty") != std::string::npos) {
        os << "TARGET_TAGS=fpga openocd" << std::endl;
    } else if (board.find("hifive1-revb") != std::string::npos) {
        os << "TARGET_TAGS=board jlink" << std::endl;
    } else {
        os << "TARGET_TAGS=board openocd" << std::endl;
    }
}

static void show_usage(string name)
{
  std::cerr << "Usage: " << name << " <option(s)>\n"
	    << "Options:\n"
	    << "\t-h,--help\t\t\tShow this help message\n"
	    << "\t-b,--board <eg. rtl | arty | hifive1>\t\tSpecify board type\n"
	    << "\t-d,--dtb <eg. xxx.dtb>\t\tSpecify fullpath to the DTB file\n"
	    << "\t-o,--output <eg. openocd.cfg>\t\tGenerate openocd config file\n"
	    << "\t-s,--show \t\tShow openocd config file on std-output\n"
	    << endl;
}

int main (int argc, char* argv[])
{
  bool show = false;
  string board = "arty";;
  string dtb_file;
  string config_file;

  if ((argc < 2) && (argc > 5)) {
      show_usage(argv[0]);
      return 1;
  } else {
      for (int i = 1; i < argc; ++i) {
          string arg = argv[i];
          if ((arg == "-b") || (arg == "--board")) {
              if (i + 1 < argc) {
                  board = argv[++i];
		  if ((board.find("rtl")  != std::string::npos) ||
		      (board.find("arty") != std::string::npos) ||
		      (board.find("hifive1") != std::string::npos)) {
		    std::cout << "Board type given is " << board << std::endl;
                  } else {
		    std::cerr << "Possible options are <rtl | arty | hifive1>."
			      << std::endl;
		    return 1;
		  }
              }
          } else if ((arg == "-d") || (arg == "--dtb")) {
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
          } else if ((arg == "-s") || (arg == "--show")) {
	      show = true;
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

  dts_memory();

  if (!config_file.empty()) {
    std::fstream cfg;

    cfg.open(config_file, fstream::in | fstream::out | fstream::trunc);
    if (cfg.is_open() == false) {
      std::cerr << "Error: Failed to create " << config_file << std::endl;
      return 1;
    }

    write_config_file(cfg, board);
  }

  if (show) {
    show_dts_attributes();
  }

  return 0;
}
