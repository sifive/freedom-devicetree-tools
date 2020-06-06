/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_NB2_EMMC__H
#define __BARE_HEADER_SIFIVE_NB2_EMMC__H

#include "bare_header/device.h"

#include <regex>

class sifive_nb2emmc : public Device {
  public:
    sifive_nb2emmc(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,nb2emmc")
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
      emit_compat("sifive,nb2emmc");	

	emit_offset("HRS00",  0x0);
	emit_offset("HRS01",  0x4);
	emit_offset("HRS02",  0x8);
	emit_offset("HRS03",  0xc);
	emit_offset("HRS04", 0x10);
	emit_offset("HRS05", 0x14);
	emit_offset("HRS06", 0x18);
	emit_offset("HRS07", 0x1c);
	emit_offset("HRS30", 0x78);
	emit_offset("HRS31", 0X7c);
	emit_offset("HRS32", 0x80);
	emit_offset("HRS33", 0x84);
	emit_offset("HRS34", 0x88);
	emit_offset("HRS35", 0x8c);
	emit_offset("HRS36", 0x90);
	emit_offset("HRS37", 0x94);
	emit_offset("HRS38", 0x98);
	
	emit_offset("CRS63", 0xfc);
	
	emit_offset("SRS00", 0x200);
	emit_offset("SRS01", 0x204);
	emit_offset("SRS02", 0x208);
	emit_offset("SRS03", 0x20c);
	emit_offset("SRS04", 0x210);
	emit_offset("SRS05", 0x214);
	emit_offset("SRS06", 0x218);
	emit_offset("SRS07", 0x21c);
	emit_offset("SRS08", 0x220);
	emit_offset("SRS09", 0x224);
	emit_offset("SRS10", 0x228);
	emit_offset("SRS11", 0x22c);
	emit_offset("SRS12", 0x230);
	emit_offset("SRS13", 0x234);
	emit_offset("SRS14", 0x238);
	emit_offset("SRS15", 0x23c);
	emit_offset("SRS16", 0x240);
	emit_offset("SRS17", 0x244);
	emit_offset("SRS18", 0x248);
	emit_offset("SRS19", 0x24c);
	emit_offset("SRS20", 0x250);
	emit_offset("SRS21", 0x254);
	emit_offset("SRS22", 0x258);
	emit_offset("SRS23", 0x25c);
	emit_offset("SRS24", 0x260);
	emit_offset("SRS25", 0x264);
	emit_offset("SRS26", 0x268);
	emit_offset("SRS27", 0x26c);
	emit_offset("SRS28", 0x270);
	emit_offset("SRS29", 0x274);
	
	emit_offset("CQRS00", 0x400);
	emit_offset("CQRS01", 0x404);
	emit_offset("CQRS02", 0x408);
	emit_offset("CORS03", 0x40c);
	emit_offset("CQRS04", 0x410);
	emit_offset("CQRS05", 0x414);
	emit_offset("CQRS06", 0x418);
	emit_offset("CQRS07", 0x41c);
	emit_offset("CQRS08", 0x420);
	emit_offset("CQRS09", 0x424);
	emit_offset("CQRS10", 0x428);
	emit_offset("CQRS11", 0x42c);
	emit_offset("CQRS12", 0x430);
	emit_offset("CQRS13", 0x434);
	emit_offset("CQRS14", 0x438);
	emit_offset("CQRS16", 0x440);
	emit_offset("CQRS17", 0x444);
	emit_offset("CQRS18", 0x448);
	emit_offset("CQRS19", 0x44c);
	emit_offset("CQRS20", 0x450);
	emit_offset("CQRS21", 0x454);
	emit_offset("CQRS22", 0x458);
	emit_offset("CQRS23", 0x45c);

	os << std::endl;
      }
    }
};

#endif
