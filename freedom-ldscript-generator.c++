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
  int mem_start;
  int mem_length;

  memory() {}
  memory(std::string, std::string, std::string, int, int);
};

memory::memory (std::string mtype, std::string alias, std::string name, int start, int length)
{
  mem_type = mtype;
  mem_alias = alias;
  mem_name = name;
  mem_start = start;
  mem_length = length;
}

static char *dts_blob;
static std::vector<memory> dts_memory_list;

static inline bool has_memory (std::string mem)
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
		std::vector<std::string> splices = split(string(nodep), '@');
		regp = (const char *)fdt_getprop(dts_blob, offset, "reg", &reglen);
		if (reglen > 0) {
		    const fdt32_t *cell = (const fdt32_t *)regp;
		    dts_memory_list.push_back(memory(s, splices[0], splices[0],
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
    alias_memory("testram", "flash");
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

static void write_linker_file (fstream &os)
{
  //os << "#" << __FUNCTION__ << std::endl;
    os << "OUTPUT_ARCH(\"riscv\")" << std::endl << std::endl;
    os << "ENTRY(_enter)" << std::endl << std::endl;
}

static void write_linker_memory (fstream &os, bool scratchpad)
{
  //os << "#" << __FUNCTION__ << std::endl;
  int flash_offset = 0;
    os << "MEMORY" << std::endl << "{" << std::endl;
    for (auto entry : dts_memory_list) {
      if (!scratchpad &&
	  (entry.mem_type.compare("mem") == 0) &&
	  (entry.mem_alias.compare("flash") == 0)) {
        if (entry.mem_name.compare("spi") == 0)
	  flash_offset = 0x400000;
	os << "\t" << entry.mem_alias <<  " (rxai!w)";
      } else if (entry.mem_alias.compare("ram") == 0) {
	os << "\t" << entry.mem_alias <<  " (wxa!ri)";
      } else {
	continue;
      }
      os << " : ORIGIN = 0x" << std::hex << (entry.mem_start + flash_offset);
      os << ", LENGTH = 0x" << entry.mem_length;
      os << std::dec << std::endl;
    }
    os << "}" << std::endl << std::endl;
}

static void write_linker_phdrs (fstream &os, bool scratchpad)
{
  //os << "#" << __FUNCTION__ << std::endl;
    os << "PHDRS" << std::endl << "{" << std::endl;
    os << "\tflash PT_LOAD;" << std::endl;
    os << "\tram_init PT_LOAD;" << std::endl;
    if (scratchpad == false)
        os << "\tram PT_NULL;" << std::endl;
    else
        os << "\tram PT_LOAD;" << std::endl;
    os << "}" << std::endl << std::endl;
}

static void write_linker_sections (fstream &os, bool scratchpad, bool ramrodata)
{
    std::string stack_cfg = scratchpad ? "0x400" : "0x800";

  //os << "#" << __FUNCTION__ << std::endl;
    os << "SECTIONS" << std::endl << "{" << std::endl;

    /* Define stack size */
    os << "\t__stack_size = DEFINED(__stack_size) ? __stack_size : "
	      << stack_cfg << ";" << std::endl;

    os << std::endl << std::endl;
    /* Define init section */
    os << "\t.init \t\t:" << std::endl;
    os << "\t{" << std::endl;
    os << "\t\tKEEP (*(.text.mee.init.enter))" << std::endl;
    os << "\t\tKEEP (*(SORT_NONE(.init)))" << std::endl;
    if (scratchpad) {
      os << "\t} >ram AT>ram :ram" << std::endl;
    } else {
      os << "\t} >flash AT>flash :flash" << std::endl;
    }

    os << std::endl << std::endl;
    /* Define text section */
    os << "\t.text \t\t:" << std::endl;
    os << "\t{" << std::endl;
    os << "\t\t*(.text.unlikely .text.unlikely.*)" << std::endl;
    os << "\t\t*(.text.startup .text.startup.*)" << std::endl;
    os << "\t\t*(.text .text.*)" << std::endl;
    os << "\t\t*(.gnu.linkonce.t.*)" << std::endl;
    if (scratchpad) {
      os << "\t} >ram AT>ram :ram" << std::endl;
    } else {
      os << "\t} >flash AT>flash :flash" << std::endl;
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
      if (scratchpad) {
	os << "\t} >ram AT>ram :ram" << std::endl;
      } else {
	os << "\t} >flash AT>flash :flash" << std::endl;
      }
    }

    os << std::endl << std::endl;
    os << "\t. = ALIGN(4);" << std::endl;

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
    /* Define finit_array section */
    os << "\t.finit_array \t\t:" << std::endl;
    os << "\t{" << std::endl;
    os << "\t\tPROVIDE_HIDDEN (__finit_array_start = .);" << std::endl;
    os << "\t\tKEEP (*(SORT_BY_INIT_PRIORITY(.fini_array.*) SORT_BY_INIT_PRIORITY(.dtors.*)))" << std::endl;
    os << "\t\tKEEP (*(.fini_array EXCLUDE_FILE (*crtbegin.o *crtbegin?.o *crtend.o *crtend?.o ) .dtors))" << std::endl;
    os << "\t\tPROVIDE_HIDDEN (__finit_array_end = .);" << std::endl;
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
    /* Define lalign section */
    os << "\t.lalign \t\t:" << std::endl;
    os << "\t{" << std::endl;
    os << "\t\t. = ALIGN(4);" << std::endl;
    os << "\t\tPROVIDE( _data_lma = . );" << std::endl;
    os << "\t\tPROVIDE( mee_segment_data_source_start = . );" << std::endl;
    if (scratchpad) {
      os << "\t} >ram AT>ram :ram" << std::endl;
    } else {
      os << "\t} >flash AT>flash :flash" << std::endl;
    }

    os << std::endl << std::endl;
    /* Define dalign section */
    os << "\t.dalign \t\t:" << std::endl;
    os << "\t{" << std::endl;
    os << "\t\t. = ALIGN(4);" << std::endl;
    os << "\t\tPROVIDE( mee_segment_data_target_start = . );" << std::endl;
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
    }
    os << "\t\t*(.data .data.*)" << std::endl;
    os << "\t\t*(.gnu.linkonce.d.*)" << std::endl;
    os << "\t\t. = ALIGN(8);" << std::endl;
    os << "\t\tPROVIDE( __global_pointer$ = . + 0x800 );" << std::endl;
    os << "\t\t*(.sdata .sdata.*)" << std::endl;
    os << "\t\t*(.gnu.linkonce.s.*)" << std::endl;
    os << "\t\t. = ALIGN(8);" << std::endl;
    os << "\t\t*(.srodata.cst16)" << std::endl;
    os << "\t\t*(.srodata.cst8)" << std::endl;
    os << "\t\t*(.srodata.cst4)" << std::endl;
    os << "\t\t*(.srodata.cst2)" << std::endl;
    os << "\t\t*(.srodata .srodata.*)" << std::endl;
    if (scratchpad) {
      os << "\t} >ram AT>ram :ram_init" << std::endl;
    } else {
      os << "\t} >ram AT>flash :ram_init" << std::endl;
    }

    os << std::endl << std::endl;
    /* Define some more end.. labels */
    os << "\t. = ALIGN(4);" << std::endl;
    os << "\tPROVIDE( _edata = . );" << std::endl;
    os << "\tPROVIDE( edata = . );" << std::endl;
    os << "\tPROVIDE( mee_segment_data_target_end = . );" << std::endl;

    os << "\tPROVIDE( _fbss = . );" << std::endl;
    os << "\tPROVIDE( __bss_start = . );" << std::endl;
    os << "\tPROVIDE( mee_segment_bss_target_start = . );" << std::endl;

    os << std::endl << std::endl;
    /* Define bss section */
    os << "\t.bss \t\t:" << std::endl;
    os << "\t{" << std::endl;
    os << "\t\t*(.sbss*)" << std::endl;
    os << "\t\t*(.gnu.linkonce.sb.*)" << std::endl;
    os << "\t\t*(.bss .bss.*)" << std::endl;
    os << "\t\t*(.gnu.linkonce.b.*)" << std::endl;
    os << "\t\t*(COMMON)" << std::endl;
    os << "\t\t. = ALIGN(4);" << std::endl;
    os << "\t} >ram AT>ram :ram" << std::endl;

    os << std::endl << std::endl;
    /* Define end labels */
    os << "\t. = ALIGN(8);" << std::endl;
    os << "\tPROVIDE( _end = . );" << std::endl;
    os << "\tPROVIDE( end = . );" << std::endl;
    os << "\tPROVIDE( mee_segment_bss_target_end = . );" << std::endl;
    os << "\tPROVIDE( mee_segment_heap_target_start = . );" << std::endl;

    os << std::endl << std::endl;
    /* Define stack section */
    if (scratchpad) {
      os << "\t.stack :" << std::endl;
      os << "\t{" << std::endl;
      os << "\t\t. = ALIGN(8);" << std::endl;
      os << "\t\t. += __stack_size;" << std::endl;
      os << "\t\tPROVIDE( _sp = . );" << std::endl;
      os << "\t\tPROVIDE( _heap_end = . );" << std::endl;
      os << "\t\tPROVIDE(mee_segment_stack_end = .);" << std::endl;
    } else {
      os << "\t.stack ORIGIN(ram) + LENGTH(ram) - __stack_size :" << std::endl;
      os << "\t{" << std::endl;
      os << "\t\tPROVIDE( mee_segment_heap_target_end = . );" << std::endl;;
      os << "\t\tPROVIDE( _heap_end = . );" << std::endl;;
      os << "\t\t. = __stack_size;" << std::endl;
      os << "\t\tPROVIDE( _sp = . );" << std::endl;
      os << "\t\tPROVIDE(mee_segment_stack_end = .);" << std::endl;
    }
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
  bool ramrodata = false;
  bool scratchpad = false;

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
  dts_memory();

  if (has_memory("spi") == 0 && has_memory("testram"))
    scratchpad = true;

  if (!linker_file.empty()) {
    std::fstream lds;

    lds.open(linker_file, fstream::in | fstream::out | fstream::trunc);
    if (lds.is_open() == false) {
      std::cerr << "Error: Failed to create " << linker_file << std::endl;
      return 1;
    }

    write_linker_file(lds);
    write_linker_memory(lds, scratchpad);
    write_linker_phdrs(lds, scratchpad);
    write_linker_sections(lds, scratchpad, ramrodata);
  }

  if (!show.empty()) {
    show_dts_memory(show);
  }

  return 0;
}
