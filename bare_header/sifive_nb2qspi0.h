/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_NB2_QSPI0__H
#define __BARE_HEADER_SIFIVE_NB2_QSPI0__H

#include "bare_header/device.h"

#include <regex>

class sifive_nb2qspi0 : public Device {
  public:
    sifive_nb2qspi0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,nb2qspi0")
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
	emit_compat("sifive,nb2qspi0");
	
	emit_offset("COMMAND_REG", 0x0);
	emit_offset("ADDRESS_REG", 0x4);
	emit_offset("DUMMY_DLP_REG", 0x8);
	emit_offset("MODE_REG", 0xC);
	emit_offset("CMD_CFG_REG", 0x10);
	emit_offset("TX_DATA_REG", 0x14);
	emit_offset("RX_DATA_REG", 0x18);
	emit_offset("START_CMD_REG", 0x1C);
	emit_offset("CUSTOM_CMD_REG", 0x20);
	emit_offset("INTR_STAT_REG", 0x24);
	emit_offset("INTR_MASK_CLR_REG", 0x28);
	emit_offset("BAUD_RATE_REG", 0x2C);
	emit_offset("BURST_CTRL_REG", 0x30);
	emit_offset("SYSTEM_STATUS_REG", 0x34);

	os << std::endl;
      }
    }

};

#endif
