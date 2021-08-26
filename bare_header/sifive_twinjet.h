/* Copyright 2021 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __BARE_HEADER_SIFIVE_TWINJET__H
#define __BARE_HEADER_SIFIVE_TWINJET__H

#include "bare_header/device.h"

#include <regex>

#define TWINJET_PREFIX "METAL_SIFIVE_TWINJET"

class sifive_twinjet : public Device {
private:
  bool skip;

public:
  sifive_twinjet(std::ostream &os, const fdt &dtb)
      : Device(os, dtb, "hwpf-nStreams"), skip(false) {}

  void emit_defines() {
    dtb.match(std::regex("cpu"), [&](const node n) {
      n.maybe_tuple(
          compat_string, tuple_t<uint32_t>(), []() {},
          [&](uint32_t num) {
            if (!skip) {
              emit_compat("SIFIVE_TWINJET");

              emit_property_u32(TWINJET_PREFIX, METAL_SIFIVE_TWINJET_WINDOWBITS_LABEL,
                                n.get_field<uint32_t>("hwpf-windowBits"));
              emit_property_u32(TWINJET_PREFIX, METAL_SIFIVE_TWINJET_DISTANCEBITS_LABEL,
                                n.get_field<uint32_t>("hwpf-distanceBits"));
              emit_property_u32(
                  TWINJET_PREFIX, METAL_SIFIVE_TWINJET_QFULLNESSTHRDBITS_LABEL,
                  n.get_field<uint32_t>("hwpf-qFullnessThrdBits"));
              emit_property_u32(TWINJET_PREFIX,
                                METAL_SIFIVE_TWINJET_HITCACHETHRDBITS_LABEL,
                                n.get_field<uint32_t>("hwpf-hitCacheThrdBits"));
              emit_property_u32(TWINJET_PREFIX, METAL_SIFIVE_TWINJET_HITMSHRTHRDBITS_LABEL,
                                n.get_field<uint32_t>("hwpf-hitMSHRThrdBits"));
              emit_property_u32(TWINJET_PREFIX, METAL_SIFIVE_TWINJET_NSTREAMS_LABEL,
                                n.get_field<uint32_t>("hwpf-nStreams"));
              emit_property_u32(TWINJET_PREFIX, METAL_SIFIVE_TWINJET_NISSQENT_LABEL,
                                n.get_field<uint32_t>("hwpf-nIssQEnt"));
              emit_property_u32(TWINJET_PREFIX, METAL_SIFIVE_TWINJET_L2PFPOOLSIZE_LABEL,
                                n.get_field<uint32_t>("hwpf-l2pfPoolSize"));
              emit_property_u32(
                  TWINJET_PREFIX, METAL_SIFIVE_TWINJET_NPREPFETCHQUEUEENTRIES_LABEL,
                  n.get_field<uint32_t>("hwpf-nPrefetchQueueEntries"));

              os << std::endl;
            }
            skip = true;
          });
    });
  }
};

#endif /* __BARE_HEADER_SIFIVE_TWINJET__H */
