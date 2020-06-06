/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_NB2_UART0__H
#define __BARE_HEADER_SIFIVE_NB2_UART0__H

#include "bare_header/device.h"

#include <regex>

class sifive_nb2uart0 : public Device {
  public:
    sifive_nb2uart0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,nb2uart0")
    {}

    void emit_defines() {
      dtb.match(
	std::regex(compat_string),
	[&](node n) {
	  emit_comment(n);

	  emit_base(n);
	  emit_size(n);

	  os << std::endl;
	});
    }

    void emit_offsets() {
      if(dtb.match(std::regex(compat_string), [](const node n){}) != 0) {
      emit_compat("sifive,nb2uart0");
	emit_offset("RBR",0x0);
	emit_offset("DLL",0x0);
	emit_offset("THR",0x0);
	emit_offset("DLH",0x4);
	emit_offset("IER",0x4);
	emit_offset("FCR",0x8);
	emit_offset("IIR",0x8);
	emit_offset("LCR",0xc);
	emit_offset("MCR",0x10);
	emit_offset("LSR",0x14);
	emit_offset("MSR",0x18);
	emit_offset("SCR",0x1c);
	emit_offset("LPDLL",0x20);
	emit_offset("LPDLH",0x24);
	emit_offset("FAR",0x70);
	emit_offset("TFR",0x74);
	emit_offset("RFW",0x78);
	emit_offset("USR",0x7c);
	emit_offset("TFL",0x80);
	emit_offset("RFL",0x84);
	emit_offset("SRR",0x88);
	emit_offset("SRTS" ,0x8c);
	emit_offset("SBCR" ,0x90);
	emit_offset("SDMAM" ,0x94);
	emit_offset("SFE" ,0x98);
	emit_offset("SRT" ,0x9c);
	emit_offset("STET" ,0xa0);
	emit_offset("HTX" ,0xa4);
	emit_offset("DMASA" ,0xa8);
	emit_offset("TCR" ,0xac);
	emit_offset("DE_EN" ,0xb0);
	emit_offset("RE_EN" ,0xb4);
	emit_offset("DET" ,0xb8);
	emit_offset("TAT" ,0xbc);
	emit_offset("DLF",0xc0);
	emit_offset("RAR",0xc4);
	emit_offset("TAR" ,0xc8);
	emit_offset("LCR_EXT" ,0xcc);
	emit_offset("UART_PROT_LEVEL",0xd0);
	emit_offset("REG_TIMEOUT_RST",0xd4);
	emit_offset("CPR",0xf4);
	emit_offset("UCV",0xf8);
	emit_offset("CTR",0xfc);


	os << std::endl;
      }
    }
};

#endif
