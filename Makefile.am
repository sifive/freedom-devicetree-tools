# Copyright 2018 SiFive, Inc
# SPDX-License-Identifier: Apache-2.0

# A linker script generator for SiFive's Freedom platform.
bin_PROGRAMS =  freedom-metal_header-generator \
		freedom-bare_header-generator

freedom_metal_header_generator_CPPFLAGS = -I$(srcdir)/metal_header

freedom_metal_header_generator_SOURCES = \
	fdt.c++ \
	header-labels.h \
	metal_header/device.c++ \
	metal_header/device.h \
	metal_header/fixed_clock.c++ \
	metal_header/fixed_clock.h \
	metal_header/fixed_factor_clock.c++ \
	metal_header/fixed_factor_clock.h \
	metal_header/freedom-metal_header-generator.c++ \
	metal_header/inline.h \
	metal_header/memory.h \
	metal_header/memory.c++ \
	metal_header/riscv_clint0.c++ \
	metal_header/riscv_clint0.h \
	metal_header/riscv_cpu.c++ \
	metal_header/riscv_cpu.h \
	metal_header/riscv_cpu_intc.c++ \
	metal_header/riscv_cpu_intc.h \
	metal_header/riscv_plic0.c++ \
	metal_header/riscv_plic0.h \
	metal_header/riscv_pmp.c++ \
	metal_header/riscv_pmp.h \
	metal_header/sifive_buserror0.c++ \
	metal_header/sifive_buserror0.h \
	metal_header/sifive_ccache0.c++ \
	metal_header/sifive_ccache0.h \
	metal_header/sifive_clic0.c++ \
	metal_header/sifive_clic0.h \
	metal_header/sifive_fe310_g000_hfrosc.c++ \
	metal_header/sifive_fe310_g000_hfrosc.h \
	metal_header/sifive_fe310_g000_hfxosc.c++ \
	metal_header/sifive_fe310_g000_hfxosc.h \
	metal_header/sifive_fe310_g000_lfrosc.c++ \
	metal_header/sifive_fe310_g000_lfrosc.h \
	metal_header/sifive_fe310_g000_pll.c++ \
	metal_header/sifive_fe310_g000_pll.h \
	metal_header/sifive_fe310_g000_prci.c++ \
	metal_header/sifive_fe310_g000_prci.h \
	metal_header/sifive_global_external_interrupts0.c++ \
	metal_header/sifive_global_external_interrupts0.h \
	metal_header/sifive_gpio0.c++ \
	metal_header/sifive_gpio0.h \
	metal_header/sifive_gpio_button.c++ \
	metal_header/sifive_gpio_button.h \
	metal_header/sifive_gpio_led.c++ \
	metal_header/sifive_gpio_led.h \
	metal_header/sifive_gpio_switch.c++ \
	metal_header/sifive_gpio_switch.h \
	metal_header/sifive_i2c0.c++ \
	metal_header/sifive_i2c0.h \
	metal_header/sifive_local_external_interrupts0.c++ \
	metal_header/sifive_local_external_interrupts0.h \
	metal_header/sifive_l2pf0.c++ \
	metal_header/sifive_l2pf0.h \
	metal_header/sifive_pwm0.c++ \
	metal_header/sifive_pwm0.h \
	metal_header/sifive_rtc0.c++ \
	metal_header/sifive_rtc0.h \
	metal_header/sifive_spi0.c++ \
	metal_header/sifive_spi0.h \
	metal_header/sifive_test0.c++ \
	metal_header/sifive_test0.h \
	metal_header/sifive_trace.c++ \
	metal_header/sifive_trace.h \
	metal_header/sifive_uart0.c++ \
	metal_header/sifive_uart0.h \
	metal_header/sifive_simuart0.c++ \
	metal_header/sifive_simuart0.h \
	metal_header/sifive_wdog0.c++ \
	metal_header/sifive_wdog0.h \
	metal_header/stdout_path.c++ \
	metal_header/stdout_path.h \
	metal_header/template_device.c++ \
	metal_header/template_device.h \
	metal_header/ucb_htif0.c++ \
	metal_header/ucb_htif0.h

freedom_bare_header_generator_CPPFLAGS = -I$(srcdir) -I$(srcdir)/bare_header

freedom_bare_header_generator_SOURCES = \
	fdt.c++ \
	header-labels.h \
	bare_header/freedom-bare_header-generator.c++ \
	bare_header/device.c++ \
	bare_header/device.h \
	bare_header/fixed_clock.h \
	bare_header/fixed_factor_clock.h \
	bare_header/riscv_clint0.h \
	bare_header/riscv_plic0.h \
	bare_header/sifive_aon0.h \
	bare_header/sifive_buserror0.h \
	bare_header/sifive_ccache0.h \
	bare_header/sifive_clic0.h \
	bare_header/sifive_error0.h \
	bare_header/sifive_fe310_g000_hfrosc.h \
	bare_header/sifive_fe310_g000_hfxosc.h \
	bare_header/sifive_fe310_g000_lfrosc.h \
	bare_header/sifive_fe310_g000_prci.h \
	bare_header/sifive_fe310_g000_pll.h \
	bare_header/sifive_global_external_interrupts0.h \
	bare_header/sifive_gpio0.h \
	bare_header/sifive_gpio_buttons.h \
	bare_header/sifive_gpio_leds.h \
	bare_header/sifive_gpio_switches.h \
	bare_header/sifive_i2c0.h \
	bare_header/sifive_local_external_interrupts0.h \
	bare_header/sifive_l2pf0.h \
	bare_header/sifive_pwm0.h \
	bare_header/sifive_rtc0.h \
	bare_header/sifive_spi0.h \
	bare_header/sifive_test0.h \
	bare_header/sifive_trace.h \
	bare_header/sifive_uart0.h \
	bare_header/sifive_simuart0.h \
	bare_header/sifive_wdog0.h \
	bare_header/ucb_htif0.h \
	bare_header/sifive_hca_0_5_x.h \
	ranges.c++ \
	ranges.h

AM_TESTS_ENVIRONMENT = \
  export SOURCE_DIR=$(abspath $(srcdir)); \
  export WORK_DIR=$(abspath .);

# Generic test scripts called by tests
TESTS       =
EXTRA_DIST  = tests/test-bare_header-generator.bash
EXTRA_DIST += tests/test-metal_header-generator.bash

# freedom-e310-arty
EXTRA_DIST += tests/freedom-e310-arty/core.dts
EXTRA_DIST += tests/freedom-e310-arty/design.dts
TESTS      += tests/freedom-e310-arty/test-bare_header-generator.bash
TESTS      += tests/freedom-e310-arty/test-metal_header-generator.bash
# qemu-sifive-e31
EXTRA_DIST += tests/qemu-sifive-e31/core.dts
EXTRA_DIST += tests/qemu-sifive-e31/design.dts
TESTS      += tests/qemu-sifive-e31/test-bare_header-generator.bash
TESTS      += tests/qemu-sifive-e31/test-metal_header-generator.bash
# qemu-sifive-s51
EXTRA_DIST += tests/qemu-sifive-s51/core.dts
EXTRA_DIST += tests/qemu-sifive-s51/design.dts
TESTS      += tests/qemu-sifive-s51/test-bare_header-generator.bash
TESTS      += tests/qemu-sifive-s51/test-metal_header-generator.bash
# qemu-sifive-u54
EXTRA_DIST += tests/qemu-sifive-u54/core.dts
EXTRA_DIST += tests/qemu-sifive-u54/design.dts
TESTS      += tests/qemu-sifive-u54/test-bare_header-generator.bash
TESTS      += tests/qemu-sifive-u54/test-metal_header-generator.bash
# qemu-sifive-u54mc
EXTRA_DIST += tests/qemu-sifive-u54mc/core.dts
EXTRA_DIST += tests/qemu-sifive-u54mc/design.dts
TESTS      += tests/qemu-sifive-u54mc/test-bare_header-generator.bash
TESTS      += tests/qemu-sifive-u54mc/test-metal_header-generator.bash
# sifive-hifive1
EXTRA_DIST += tests/sifive-hifive1/core.dts
EXTRA_DIST += tests/sifive-hifive1/design.dts
TESTS      += tests/sifive-hifive1/test-bare_header-generator.bash
TESTS      += tests/sifive-hifive1/test-metal_header-generator.bash
# sifive-hifive1-revb
EXTRA_DIST += tests/sifive-hifive1-revb/core.dts
EXTRA_DIST += tests/sifive-hifive1-revb/design.dts
TESTS      += tests/sifive-hifive1-revb/test-bare_header-generator.bash
TESTS      += tests/sifive-hifive1-revb/test-metal_header-generator.bash
# sifive-hifive-unleashed
EXTRA_DIST += tests/sifive-hifive-unleashed/core.dts
EXTRA_DIST += tests/sifive-hifive-unleashed/design.dts
TESTS      += tests/sifive-hifive-unleashed/test-bare_header-generator.bash
TESTS      += tests/sifive-hifive-unleashed/test-metal_header-generator.bash
# spike
EXTRA_DIST += tests/spike/core.dts
EXTRA_DIST += tests/spike/design.dts
TESTS      += tests/spike/test-bare_header-generator.bash
TESTS      += tests/spike/test-metal_header-generator.bash
# e20-arty
EXTRA_DIST += tests/e20-arty/core.dts
EXTRA_DIST += tests/e20-arty/design.dts
TESTS      += tests/e20-arty/test-bare_header-generator.bash
TESTS      += tests/e20-arty/test-metal_header-generator.bash
# e20-rtl
EXTRA_DIST += tests/e20-rtl/core.dts
EXTRA_DIST += tests/e20-rtl/design.dts
TESTS      += tests/e20-rtl/test-bare_header-generator.bash
TESTS      += tests/e20-rtl/test-metal_header-generator.bash
# e21-arty
EXTRA_DIST += tests/e21-arty/core.dts
EXTRA_DIST += tests/e21-arty/design.dts
TESTS      += tests/e21-arty/test-bare_header-generator.bash
TESTS      += tests/e21-arty/test-metal_header-generator.bash
# e21-rtl
EXTRA_DIST += tests/e21-rtl/core.dts
EXTRA_DIST += tests/e21-rtl/design.dts
TESTS      += tests/e21-rtl/test-bare_header-generator.bash
TESTS      += tests/e21-rtl/test-metal_header-generator.bash
# e24-arty
EXTRA_DIST += tests/e24-arty/core.dts
EXTRA_DIST += tests/e24-arty/design.dts
TESTS      += tests/e24-arty/test-bare_header-generator.bash
TESTS      += tests/e24-arty/test-metal_header-generator.bash
# e24-rtl
EXTRA_DIST += tests/e24-rtl/core.dts
EXTRA_DIST += tests/e24-rtl/design.dts
TESTS      += tests/e24-rtl/test-bare_header-generator.bash
TESTS      += tests/e24-rtl/test-metal_header-generator.bash
# e31-arty
EXTRA_DIST += tests/e31-arty/core.dts
EXTRA_DIST += tests/e31-arty/design.dts
TESTS      += tests/e31-arty/test-bare_header-generator.bash
TESTS      += tests/e31-arty/test-metal_header-generator.bash
# e31-rtl
EXTRA_DIST += tests/e31-rtl/core.dts
EXTRA_DIST += tests/e31-rtl/design.dts
TESTS      += tests/e31-rtl/test-bare_header-generator.bash
TESTS      += tests/e31-rtl/test-metal_header-generator.bash
# e34-arty
EXTRA_DIST += tests/e34-arty/core.dts
EXTRA_DIST += tests/e34-arty/design.dts
TESTS      += tests/e34-arty/test-bare_header-generator.bash
TESTS      += tests/e34-arty/test-metal_header-generator.bash
# e34-rtl
EXTRA_DIST += tests/e34-rtl/core.dts
EXTRA_DIST += tests/e34-rtl/design.dts
TESTS      += tests/e34-rtl/test-bare_header-generator.bash
TESTS      += tests/e34-rtl/test-metal_header-generator.bash
# e76-arty
EXTRA_DIST += tests/e76-arty/core.dts
EXTRA_DIST += tests/e76-arty/design.dts
TESTS      += tests/e76-arty/test-bare_header-generator.bash
TESTS      += tests/e76-arty/test-metal_header-generator.bash
# e76mc-arty
EXTRA_DIST += tests/e76mc-arty/core.dts
EXTRA_DIST += tests/e76mc-arty/design.dts
TESTS      += tests/e76mc-arty/test-bare_header-generator.bash
TESTS      += tests/e76mc-arty/test-metal_header-generator.bash
# e76mc-rtl
EXTRA_DIST += tests/e76mc-rtl/core.dts
EXTRA_DIST += tests/e76mc-rtl/design.dts
TESTS      += tests/e76mc-rtl/test-bare_header-generator.bash
TESTS      += tests/e76mc-rtl/test-metal_header-generator.bash
# e76-rtl
EXTRA_DIST += tests/e76-rtl/core.dts
EXTRA_DIST += tests/e76-rtl/design.dts
TESTS      += tests/e76-rtl/test-bare_header-generator.bash
TESTS      += tests/e76-rtl/test-metal_header-generator.bash
# s21-arty
EXTRA_DIST += tests/s21-arty/core.dts
EXTRA_DIST += tests/s21-arty/design.dts
TESTS      += tests/s21-arty/test-bare_header-generator.bash
TESTS      += tests/s21-arty/test-metal_header-generator.bash
# s21-rtl
EXTRA_DIST += tests/s21-rtl/core.dts
EXTRA_DIST += tests/s21-rtl/design.dts
TESTS      += tests/s21-rtl/test-bare_header-generator.bash
TESTS      += tests/s21-rtl/test-metal_header-generator.bash
# s51-arty
EXTRA_DIST += tests/s51-arty/core.dts
EXTRA_DIST += tests/s51-arty/design.dts
TESTS      += tests/s51-arty/test-bare_header-generator.bash
TESTS      += tests/s51-arty/test-metal_header-generator.bash
# s51-rtl
EXTRA_DIST += tests/s51-rtl/core.dts
EXTRA_DIST += tests/s51-rtl/design.dts
TESTS      += tests/s51-rtl/test-bare_header-generator.bash
TESTS      += tests/s51-rtl/test-metal_header-generator.bash
# s54-arty
EXTRA_DIST += tests/s54-arty/core.dts
EXTRA_DIST += tests/s54-arty/design.dts
TESTS      += tests/s54-arty/test-bare_header-generator.bash
TESTS      += tests/s54-arty/test-metal_header-generator.bash
# s54-rtl
EXTRA_DIST += tests/s54-rtl/core.dts
EXTRA_DIST += tests/s54-rtl/design.dts
TESTS      += tests/s54-rtl/test-bare_header-generator.bash
TESTS      += tests/s54-rtl/test-metal_header-generator.bash
# s76-arty
EXTRA_DIST += tests/s76-arty/core.dts
EXTRA_DIST += tests/s76-arty/design.dts
TESTS      += tests/s76-arty/test-bare_header-generator.bash
TESTS      += tests/s76-arty/test-metal_header-generator.bash
# s76mc-arty
EXTRA_DIST += tests/s76mc-arty/core.dts
EXTRA_DIST += tests/s76mc-arty/design.dts
TESTS      += tests/s76mc-arty/test-bare_header-generator.bash
TESTS      += tests/s76mc-arty/test-metal_header-generator.bash
# s76mc-rtl
EXTRA_DIST += tests/s76mc-rtl/core.dts
EXTRA_DIST += tests/s76mc-rtl/design.dts
TESTS      += tests/s76mc-rtl/test-bare_header-generator.bash
TESTS      += tests/s76mc-rtl/test-metal_header-generator.bash
# s76-rtl
EXTRA_DIST += tests/s76-rtl/core.dts
EXTRA_DIST += tests/s76-rtl/design.dts
TESTS      += tests/s76-rtl/test-bare_header-generator.bash
TESTS      += tests/s76-rtl/test-metal_header-generator.bash
# u54-arty
EXTRA_DIST += tests/u54-arty/core.dts
EXTRA_DIST += tests/u54-arty/design.dts
TESTS      += tests/u54-arty/test-bare_header-generator.bash
TESTS      += tests/u54-arty/test-metal_header-generator.bash
# u54mc-arty
EXTRA_DIST += tests/u54mc-arty/core.dts
EXTRA_DIST += tests/u54mc-arty/design.dts
TESTS      += tests/u54mc-arty/test-bare_header-generator.bash
TESTS      += tests/u54mc-arty/test-metal_header-generator.bash
# u54mc-rtl
EXTRA_DIST += tests/u54mc-rtl/core.dts
EXTRA_DIST += tests/u54mc-rtl/design.dts
TESTS      += tests/u54mc-rtl/test-bare_header-generator.bash
TESTS      += tests/u54mc-rtl/test-metal_header-generator.bash
# u54-rtl
EXTRA_DIST += tests/u54-rtl/core.dts
EXTRA_DIST += tests/u54-rtl/design.dts
TESTS      += tests/u54-rtl/test-bare_header-generator.bash
TESTS      += tests/u54-rtl/test-metal_header-generator.bash
# u74-arty
EXTRA_DIST += tests/u74-arty/core.dts
EXTRA_DIST += tests/u74-arty/design.dts
TESTS      += tests/u74-arty/test-bare_header-generator.bash
TESTS      += tests/u74-arty/test-metal_header-generator.bash
# u74mc-arty
EXTRA_DIST += tests/u74mc-arty/core.dts
EXTRA_DIST += tests/u74mc-arty/design.dts
TESTS      += tests/u74mc-arty/test-bare_header-generator.bash
TESTS      += tests/u74mc-arty/test-metal_header-generator.bash
# u74mc-rtl
EXTRA_DIST += tests/u74mc-rtl/core.dts
EXTRA_DIST += tests/u74mc-rtl/design.dts
TESTS      += tests/u74mc-rtl/test-bare_header-generator.bash
TESTS      += tests/u74mc-rtl/test-metal_header-generator.bash
# u74-rtl
EXTRA_DIST += tests/u74-rtl/core.dts
EXTRA_DIST += tests/u74-rtl/design.dts
TESTS      += tests/u74-rtl/test-bare_header-generator.bash
TESTS      += tests/u74-rtl/test-metal_header-generator.bash

EXTRA_DIST += $(TESTS)

EXTRA_DIST += README.md
EXTRA_DIST += fdt.h++
EXTRA_DIST += libfdt.h++
