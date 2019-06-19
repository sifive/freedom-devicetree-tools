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

static void get_dts_attribute (const string path, const string tag)
{
    int node, len;
    const char *value;

    node = fdt_path_offset(dts_blob, path.c_str());
    value = (const char *)fdt_getprop(dts_blob, node, tag.c_str(), &len);
    std::cout << string(value) << std::endl;
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
    
    int memory_count = 0;
    int sram_count = 0;
    int dtim_count = 0;

    for (auto it = dts_memory_list.begin(); it != dts_memory_list.end(); ++it) {
      if (it->mem_alias.find("memory") != std::string::npos) {
	memory_count += 1;
      } else if (it->mem_alias.find("sram") != std::string::npos) {
	sram_count += 1;
      } else if (it->mem_alias.find("dtim") != std::string::npos) {
	dtim_count += 1;
      }
    }

    if (memory_count > 0) {
      alias_memory("memory", "ram");
    } else if (sram_count > 1) {
      alias_memory("sram0", "ram");
    } else if (dtim_count > 0) {
      alias_memory("dtim", "ram");
    } else {
      std::cerr << "Unable to find a suitable memory for RAM" << std::endl;
      exit(1);
    }

    alias_memory("spi", "flash");
}

static void show_dts_memory (void)
{
    std::cout << "#" << __FUNCTION__ << std::endl;
    std::cout << std::endl << std::endl;
    std::cout << "MEMORY" << std::endl << "{" << std::endl;
    for (auto entry : dts_memory_list) {
	std::cout << "\t" << entry.mem_alias << "/" << entry.mem_name;
	std::cout << ((entry.mem_type.compare("control") == 0) ? " (rw)" : " (rwx)");
	std::cout << " : ORIGIN = 0x" << std::hex << entry.mem_start;
	std::cout << ", LENGTH = 0x" << entry.mem_length;
	std::cout << std::dec << std::endl;
    }
    std::cout << "}" << std::endl;
}

static void write_config_file (fstream &os, std::string board)
{
    bool mem_found;
    bool cntrl_found;
    memory spi_mem;
    memory spi_cntrl;

    os << "#" << __FUNCTION__ << std::endl;
    os << "# JTAG adapter setup" << std::endl;
    os << "adapter_khz     10000" << std::endl << std::endl;

    os << "interface ftdi" << std::endl;
    if (board.compare("hifive1") == 0) {
      os << "ftdi_device_desc \"Dual RS232-HS\"" << std::endl;
      os << "ftdi_vid_pid 0x0403 0x6010" << std::endl << std::endl;

      os << "ftdi_layout_init 0x0008 0x001b" << std::endl;
      os << "ftdi_layout_signal nSRST -oe 0x0020" << std::endl;
      os << "ftdi_layout_signal LED -data 0x0020" << std::endl << std::endl;
    } else {
      /* Arty Board by Default */
      os << "ftdi_device_desc \"Olimex OpenOCD JTAG ARM-USB-TINY-H\"" << std::endl;
      os << "ftdi_vid_pid 0x15ba 0x002a" << std::endl << std::endl;

      os << "ftdi_layout_init 0x0808 0x0a1b" << std::endl;
      os << "ftdi_layout_signal nSRST -oe 0x0200" << std::endl;
      os << "ftdi_layout_signal LED -data 0x0800" << std::endl << std::endl;
    }
    os << "set _CHIPNAME riscv" << std::endl;
    os << "jtag newtap $_CHIPNAME cpu -irlen 5" << std::endl << std::endl;

    os << "set _TARGETNAME $_CHIPNAME.cpu" << std::endl;
    os << "target create $_TARGETNAME.0 riscv -chain-position $_TARGETNAME" << std::endl;
    mem_found = find_memory("dtim", "mem", spi_mem);
    os << "$_TARGETNAME.0 configure -work-area-phys 0x"
       << std::hex << (mem_found ? spi_mem.mem_start : 0) << std::dec;
    os << " -work-area-size 10000 -work-area-backup 1" << std::endl << std::endl;

    mem_found = find_memory("spi", "mem", spi_mem);
    cntrl_found =find_memory("spi", "control", spi_cntrl);
    os << "flash bank spi0 fespi 0x"
       << std::hex << (mem_found ? spi_mem.mem_start : 0);
    os << " 0 0 0 $_TARGETNAME.0 0x"
       << (cntrl_found ? spi_cntrl.mem_base : 0) << std::dec << std::endl;
    os << "init" << std::endl;
    os << "if {[ info exists pulse_srst]} {" << std::endl;
    os << "\tftdi_set_signal nSRST 0" << std::endl;
    os << "\tftdi_set_signal nSRST z" << std::endl;
    os << "sleep 1500" << std::endl;
    os << "}" << std::endl;
    os << "halt" << std::endl << std::endl;

    os << "flash protect 0 64 last off" << std::endl;
    os << "echo \"Ready for Remote Connections\"" << std::endl;
}

static void show_usage(string name)
{
  std::cerr << "Usage: " << name << " <option(s)>\n"
	    << "Options:\n"
	    << "\t-h,--help\t\t\tShow this help message\n"
	    << "\t-b,--board <eg. arty | hifive1>\t\tSpecify board type\n"
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
		  if ((board.compare("arty") != 0) &&
		      (board.compare("hifive1") != 0)) {
		    std::cerr << "Possible options are <arty | hifive1>."
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
    show_dts_memory();
  }

  return 0;
}
