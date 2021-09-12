/* Copyright 2021 SiFive, Inc */
/* SPDX-License-Identifier: Apache-2.0 */

#include <sifive_mallard0.h>
#include <stdio.h>

#define MALLARD_TWINJET_PREFIX(label)                                          \
  std::string("METAL_MALLARD0_TWINJET_") + std::string(label)

sifive_mallard0::sifive_mallard0(std::ostream &os, const fdt &dtb)
    : Device(os, dtb, "sifive,mallard0"), skip(false) {}

void sifive_mallard0::create_defines() {
  skip = false;
  dtb.match(
      std::regex(compat_string),
      [&](const node n) {
        if (!skip) {
          emit_def(
              MALLARD_TWINJET_PREFIX(METAL_MALLARD0_TWINJET_WINDOWBITS_LABEL),
              std::to_string(n.get_field<uint32_t>("hwpf-windowBits")));
          emit_def(
              MALLARD_TWINJET_PREFIX(METAL_MALLARD0_TWINJET_DISTANCEBITS_LABEL),
              std::to_string(n.get_field<uint32_t>("hwpf-distanceBits")));
          emit_def(
              MALLARD_TWINJET_PREFIX(
                  METAL_MALLARD0_TWINJET_QFULLNESSTHRDBITS_LABEL),
              std::to_string(n.get_field<uint32_t>("hwpf-qFullnessThrdBits")));
          emit_def(
              MALLARD_TWINJET_PREFIX(
                  METAL_MALLARD0_TWINJET_HITCACHETHRDBITS_LABEL),
              std::to_string(n.get_field<uint32_t>("hwpf-hitCacheThrdBits")));
          emit_def(
              MALLARD_TWINJET_PREFIX(
                  METAL_MALLARD0_TWINJET_HITMSHRTHRDBITS_LABEL),
              std::to_string(n.get_field<uint32_t>("hwpf-hitMSHRThrdBits")));
          emit_def(
              MALLARD_TWINJET_PREFIX(METAL_MALLARD0_TWINJET_NSTREAMS_LABEL),
              std::to_string(n.get_field<uint32_t>("hwpf-nStreams")));
          emit_def(
              MALLARD_TWINJET_PREFIX(METAL_MALLARD0_TWINJET_NISSQENT_LABEL),
              std::to_string(n.get_field<uint32_t>("hwpf-nIssQEnt")));
          emit_def(
              MALLARD_TWINJET_PREFIX(METAL_MALLARD0_TWINJET_L2PFPOOLSIZE_LABEL),
              std::to_string(n.get_field<uint32_t>("hwpf-l2pfPoolSize")));
          emit_def(MALLARD_TWINJET_PREFIX(
                       METAL_MALLARD0_TWINJET_NPREPFETCHQUEUEENTRIES_LABEL),
                   std::to_string(
                       n.get_field<uint32_t>("hwpf-nPrefetchQueueEntries")));

          os << std::endl;
        }
        skip = true;
      },
      true);
}

void sifive_mallard0::include_headers() {
  skip = false;
  dtb.match(std::regex(compat_string),
            [&](node n) {
              if (!skip)
                emit_include("sifive,mallard0");
              skip = true;
            },
            true);
}
