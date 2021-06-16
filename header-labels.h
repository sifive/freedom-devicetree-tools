/* Copyright (c) 2019 SiFive Inc. */
/* SPDX-License-Identifier: Apache-2.0 */

#ifndef __HEADER_LABELS__H
#define __HEADER_LABELS__H

/* This file contains the suffixes for the "bare header" #defines so that
 * they can be kept synchronized between freedom-bare_header-generator and
 * freedom-metal_header-generator. */

/* Generic Devices */
#define METAL_BASE_ADDRESS_LABEL "BASE_ADDRESS"
#define METAL_SIZE_LABEL "SIZE"

/* Clocks */
#define METAL_CLOCK_FREQUENCY_LABEL "CLOCK_FREQUENCY"
#define METAL_CLOCK_DIV_LABEL "CLOCK_DIV"
#define METAL_CLOCK_MULT_LABEL "CLOCK_MULT"

/* FE310-G000 PRCI */
#define METAL_HFROSCCFG_LABEL "HFROSCCFG"
#define METAL_HFXOSCCFG_LABEL "HFXOSCCFG"
#define METAL_PLLCFG_LABEL "PLLCFG"
#define METAL_PLLOUTDIV_LABEL "PLLOUTDIV"

/* CLINT */
#define METAL_MSIP_BASE_LABEL "MSIP_BASE"
#define METAL_MTIMECMP_BASE_LABEL "MTIMECMP_BASE"
#define METAL_MTIME_LABEL "MTIME"

/* PLIC */
#define METAL_RISCV_MAX_PRIORITY_LABEL "RISCV_MAX_PRIORITY"
#define METAL_RISCV_NDEV_LABEL "RISCV_NDEV"

/* CLIC */
#define METAL_SIFIVE_NUMINTS_LABEL "SIFIVE_NUMINTS"
#define METAL_SIFIVE_NUMLEVELS_LABEL "SIFIVE_NUMLEVELS"
#define METAL_SIFIVE_NUMINTBITS_LABEL "SIFIVE_NUMINTBITS"

/* FU540-C000 L2 */
#define METAL_SIFIVE_FU540_C000_L2_CONFIG_LABEL "CONFIG"
#define METAL_SIFIVE_FU540_C000_L2_WAYENABLE_LABEL "WAYENABLE"

/* PL2CACHE */
#define METAL_SIFIVE_PL2CACHE_CONFIG_LABEL "CONFIG"
#define METAL_SIFIVE_PL2CACHE_ECCINJECTERROR_LABEL "ECCINJECTERROR"
#define METAL_SIFIVE_PL2CACHE_ECCINJECTERRORDONE_LABEL "ECCINJECTERRORDONE"
#define METAL_SIFIVE_PL2CACHE_CFLUSH64_LABEL "CFLUSH64"
#define METAL_SIFIVE_PL2CACHE_FLUSHCOUNT_LABEL "FLUSHCOUNT"
#define METAL_SIFIVE_PL2CACHE_PMEVENTSELECT0_LABEL "PMEVENTSELECT0"
#define METAL_SIFIVE_PL2CACHE_PMCLIENTFILTER_LABEL "PMCLIENTFILTER"
#define METAL_SIFIVE_PL2CACHE_PMEVENTCOUNTER0_LABEL "PMEVENTCOUNTER0"

/* CCACHE */
#define METAL_SIFIVE_CCACHE_CONFIG_LABEL "CONFIG"
#define METAL_SIFIVE_CCACHE_WAYENABLE_LABEL "WAYENABLE"
#define METAL_SIFIVE_CCACHE_ECCINJECTERROR_LABEL "ECCINJECTERROR"
#define METAL_SIFIVE_CCACHE_DIRECCFIXLOW_LABEL "DIRECCFIXLOW"
#define METAL_SIFIVE_CCACHE_DIRECCFIXHIGH_LABEL "DIRECCFIXHIGH"
#define METAL_SIFIVE_CCACHE_DIRECCFIXCOUNT_LABEL "DIRECCFIXCOUNT"
#define METAL_SIFIVE_CCACHE_DIRECCFAILLOW_LABEL "DIRECCFAILLOW"
#define METAL_SIFIVE_CCACHE_DIRECCFAILHIGH_LABEL "DIRECCFAILHIGH"
#define METAL_SIFIVE_CCACHE_DIRECCFAILCOUNT_LABEL "DIRECCFAILCOUNT"
#define METAL_SIFIVE_CCACHE_DATECCFIXLOW_LABEL "DATECCFIXLOW"
#define METAL_SIFIVE_CCACHE_DATECCFIXHIGH_LABEL "DATECCFIXHIGH"
#define METAL_SIFIVE_CCACHE_DATECCFIXCOUNT_LABEL "DATECCFIXCOUNT"
#define METAL_SIFIVE_CCACHE_DATECCFAILLOW_LABEL "DATECCFAILLOW"
#define METAL_SIFIVE_CCACHE_DATECCFAILHIGH_LABEL "DATECCFAILHIGH"
#define METAL_SIFIVE_CCACHE_DATECCFAILCOUNT_LABEL "DATECCFAILCOUNT"
#define METAL_SIFIVE_CCACHE_FLUSH64_LABEL "FLUSH64"
#define METAL_SIFIVE_CCACHE_FLUSH32_LABEL "FLUSH32"
#define METAL_SIFIVE_CCACHE_WAYMASK0_LABEL "WAYMASK0"
#define METAL_SIFIVE_CCACHE_PMEVENTSELECT0_LABEL "PMEVENTSELECT0"
#define METAL_SIFIVE_CCACHE_PMCLIENTFILTER_LABEL "PMCLIENTFILTER"
#define METAL_SIFIVE_CCACHE_PMEVENTCOUNTER0_LABEL "PMEVENTCOUNTER0"

/* BUSERROR */
#define METAL_SIFIVE_BUSERROR_CAUSE_LABEL "CAUSE"
#define METAL_SIFIVE_BUSERROR_VALUE_LABEL "VALUE"
#define METAL_SIFIVE_BUSERROR_ENABLE_LABEL "ENABLE"
#define METAL_SIFIVE_BUSERROR_PLATFORM_INTERRUPT_LABEL "PLATFORM_INTERRUPT"
#define METAL_SIFIVE_BUSERROR_ACCRUED_LABEL "ACCRUED"
#define METAL_SIFIVE_BUSERROR_LOCAL_INTERRUPT_LABEL "LOCAL_INTERRUPT"

#endif /* __HEADER_LABELS__H */
