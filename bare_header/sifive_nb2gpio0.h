/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_NB2_GPIO0__H
#define __BARE_HEADER_SIFIVE_NB2_GPIO0__H

#include "bare_header/device.h"

#include <regex>

class sifive_nb2gpio0 : public Device {
  public:
    sifive_nb2gpio0(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "sifive,nb2gpio0")
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
	emit_compat("sifive,nb2gpio0");
	
	

	os << std::endl;
	emit_offset("GPIO_SWPORTA_DR", 0x0);
	emit_offset("GPIO_SWPORTA_DDR", 0x4);
	emit_offset("GPIO_SWPORTA_CTL", 0x8);
	emit_offset("GPIO_SWPORTB_DR", 0xC);
	emit_offset("GPIO_SWPORTB_DDR", 0x10);
	emit_offset("GPIO_SWPORTB_CTL", 0x14);
	emit_offset("GPIO_SWPORTC_DR", 0x18);
        emit_offset("GPIO_SWPORTC_DDR", 0x1C);
	emit_offset("GPIO_SWPORTC_CTL", 0x20);
	emit_offset("GPIO_SWPORTD_DR", 0x24);
        emit_offset("GPIO_SWPORTD_DDR", 0x28);
	emit_offset("GPIO_SWPORTD_CTL", 0x2C);
	emit_offset("GPIO_INTEN", 0x30);
        emit_offset("GPIO_INTMASK", 0x34);
	emit_offset("GPIO_INTTYPE_LEVEL", 0x38);
	emit_offset("GPIO_INT_POLARITY", 0x3C);
	emit_offset("GPIO_INTSTATUS", 0x40);
	emit_offset("GPIO_DEBOUNCE", 0x44);
	emit_offset("GPIO_PORTA_EOI", 0x48);

      }
    }

};

#endif
