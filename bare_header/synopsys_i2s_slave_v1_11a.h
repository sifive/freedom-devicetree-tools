/* Copyright 2019 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SYNOPSYS_I2S_SLAVE_V1_11A__H
#define __BARE_HEADER_SYNOPSYS_I2S_SLAVE_V1_11A__H

#include "bare_header/device.h"

#include <regex>

class synopsys_i2s_slave_v1_11a : public Device {
  public:
    synopsys_i2s_slave_v1_11a(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "synopsys_i2s_slave_v1_11a")
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
	emit_compat();
	/*emit_offset("IER", 0x00);
    emit_offset("IRER", 0x04);
    emit_offset("ITER", 0X08);
    emit_offset("CER", 0X0E);
    emit_offset("CCR", 0X10);
    emit_offset("RXFFR", 0X14);
    emit_offset("TXFFR", 0X18);
    emit_offset("LRBR_0", 0X020); 
    emit_offset("LRBR_1", 0X060); 
    emit_offset("LRBR_2", 0X0A0);
    emit_offset("LRBR_3", 0X0E0);
    emit_offset("LTHR_0", 0X020);
    emit_offset("LTHR_1", 0X060);
    emit_offset("LTHR_2", 0X0A0);
    emit_offset("LTHR_3", 0X0E0);
    emit_offset("RRBR_0", 0X024);
    emit_offset("RRBR_1", 0X064);
    emit_offset("RRBR_2", 0X0A4);
    emit_offset("RRBR_3", 0X0E4);
    emit_offset("RTHR_0", 0X024);
    emit_offset("RTHR_1", 0X064);
    emit_offset("RTHR_2", 0X0A4);
    emit_offset("RTHR_3", 0X0E4);
    emit_offset("RER_0", 0X028);
    emit_offset("RER_1", 0X068);
    emit_offset("RER_2", 0X0A8);
    emit_offset("RER_3", 0X0E8);
    emit_offset("TER_0", 0X02C);
    emit_offset("TER_1", 0X06C);
    emit_offset("TER_2", 0X0AC);
    emit_offset("TER_3", 0X0EC);
	emit_offset("RCR_0", 0X030);
    emit_offset("RCR_1", 0X070);
    emit_offset("RCR_2", 0X0B0);
    emit_offset("RCR_3", 0X0F0);
    emit_offset("TCR_0", 0X034);
    emit_offset("TCR_1", 0X074);
    emit_offset("TCR_2", 0X0B4);
    emit_offset("TCR_3", 0X0F4);
    emit_offset("ISR_0", 0X038);
    emit_offset("ISR_1", 0X078);
    emit_offset("ISR_2", 0X0B8);
    emit_offset("ISR_3", 0X0F8);
    emit_offset("IMR_0", 0X03C);
    emit_offset("IMR_1", 0X07C);
    emit_offset("IMR_2", 0X0BC);
    emit_offset("IMR_3", 0X0FC);
    emit_offset("ROR_0", 0X040);
    emit_offset("ROR_1", 0X080);
    emit_offset("ROR_2", 0X0C0);
    emit_offset("ROR_3", 0X100);
    emit_offset("TOR_0", 0X044);
    emit_offset("TOR_1", 0X084);
    emit_offset("TOR_2", 0X0C4);
    emit_offset("TOR_3", 0X104);
    emit_offset("RFCR_0", 0X048);
    emit_offset("RFCR_1", 0X088);
    emit_offset("RFCR_2", 0X0C8);
    emit_offset("RFCR_3", 0X108);
    emit_offset("TFCR_0", 0X04C);
    emit_offset("TFCR_1", 0X08C);
    emit_offset("TFCR_2", 0X0CC);
    emit_offset("TFCR_3", 0X10C);
    emit_offset("RFF_0", 0X050);
    emit_offset("RFF_1", 0X090);
    emit_offset("RFF_2", 0X0D0);
    emit_offset("RFF_3", 0X110);
    emit_offset("TFF_0", 0X054);
    emit_offset("TFF_1", 0X094);
    emit_offset("TFF_2", 0X0D4);
    emit_offset("TFF_3", 0X114);
    emit_offset("RXDMA", 0X1C0);
    emit_offset("RRXDMA", 0X1C4);
    emit_offset("TXDMA", 0X1C8);
    emit_offset("RTXDMA", 0X1CC);
    emit_offset("COMP_PARAM_2", 0X1F0);
    emit_offset("COMP_PARAM_1", 0X1F4);
    emit_offset("COMP_VERSION", 0X1F8);
    emit_offset("COMP_TYPE", 0X1FC);
    emit_offset("DMACR", 0X200);
    emit_offset("RXDMA_CH_0", 0X204);
    emit_offset("RXDMA_CH_1", 0X204);
    emit_offset("RXDMA_CH_2", 0X204);
    emit_offset("RXDMA_CH_3", 0X204);
    emit_offset("TXDMA_CH_0", 0X214);
    emit_offset("TXDMA_CH_1", 0X214);
    emit_offset("TXDMA_CH_2", 0X214);
    emit_offset("TXDMA_CH_3", 0X214);
    
	os << std::endl;*/
      }
    }
};

#endif
