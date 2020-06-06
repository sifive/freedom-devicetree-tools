
/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_NB2_I2C0__H
#define __BARE_HEADER_SIFIVE_NB2_I2C0__H

#include "bare_header/device.h"

#include <regex>

class synopsys_i2c_v2_02a_standard : public Device {
  public:
    synopsys_i2c_v2_02a_standard(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "synopsys,i2c_v2_02a_standard")
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
	emit_compat("synopsys,i2c_v2_02a_standard");
	emit_offset("IC_CON", 0x0);
	emit_offset("IC_TAR", 0x4);
	emit_offset("IC_SAR", 0x8);
	emit_offset("IC_HS_MADDR", 0xC);
	emit_offset("IC_DATA_CMD", 0x10);
	emit_offset("IC_SS_SCL_HCNT", 0x14);
	emit_offset("IC_SS_SCL_LCNT", 0x18);
	emit_offset("IC_FS_SCL_HCNT", 0x1C);
	emit_offset("IC_FS_SCL_LCNT", 0x20);
	emit_offset("IC_HS_SCL_HCNT", 0x24);
	emit_offset("IC_HS_SCL_LCNT", 0x28);
	emit_offset("IC_INTR_STAT", 0x2c);
	emit_offset("IC_INTR_MASK", 0x30);
	emit_offset("IC_RAW_INTR_STAT", 0x34);
	emit_offset("IC_RX_TL", 0x38);
	emit_offset("IC_TX_TL", 0x3c);
	emit_offset("IC_CLR_INTR", 0x40);
	emit_offset("IC_CLR_RX_UNDER", 0x44);
	emit_offset("IC_CLR_RX_OVER", 0x48);
	emit_offset("IC_CLR_TX_OVER", 0x4c);
	emit_offset("IC_CLR_RD_REQ", 0x50);
	emit_offset("IC_CLR_TX_ABRT", 0x54);
	emit_offset("IC_CLR_RX_DONE", 0x58);
	emit_offset("IC_CLR_ACTIVITY", 0x5c);
	emit_offset("IC_CLR_STOP_DET", 0x60);
	emit_offset("IC_CLR_START_DET", 0x64);
	emit_offset("IC_CLR_GEN_CALL", 0x68);
	emit_offset("IC_ENABLE", 0x6c);
	emit_offset("IC_STATUS",0x70);
	emit_offset("IC_SDA_HOLD",0x7C);
	emit_offset("IC_SDA_SETUP", 0x94);
	emit_offset("IC_ACK", 0x98);
	emit_offset("IC_FS_SPKLEN", 0xa0);
	emit_offset("IC_HS_SPKLEN", 0xa4);


	os << std::endl;
      }
    }

};

#endif
