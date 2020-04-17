# Copyright 2018 SiFive, Inc
# SPDX-License-Identifier: Apache-2.0

# A linker script generator for SiFive's Freedom platform.
bin_PROGRAMS =  freedom-ldscript-generator \
		freedom-openocdcfg-generator \
		freedom-makeattributes-generator \
		freedom-metal_header-generator \
		freedom-bare_header-generator \
		freedom-zephyrdtsfixup-generator \
		freedom-metal_specs-generator

freedom_ldscript_generator_SOURCES = \
	freedom-ldscript-generator.c++ \
	fdt.c++

freedom_openocdcfg_generator_SOURCES = \
	freedom-openocdcfg-generator.c++

freedom_makeattributes_generator_SOURCES = \
	freedom-makeattributes-generator.c++ \
	fdt.c++

freedom_metal_header_generator_SOURCES = \
	fdt.c++ \
	header-labels.h \
	metal_header/freedom-metal_header-generator.c++ \
	metal_header/device.c++ \
	metal_header/device.h \
	metal_header/fixed_clock.h \
	metal_header/fixed_factor_clock.h \
	metal_header/inline.h \
	metal_header/memory.h \
	metal_header/riscv_clint0.h \
	metal_header/riscv_cpu.h \
	metal_header/riscv_cpu_intc.h \
	metal_header/riscv_plic0.h \
	metal_header/riscv_pmp.h \
	metal_header/sifive_clic0.h \
	metal_header/sifive_fe310_g000_hfrosc.h \
	metal_header/sifive_fe310_g000_hfxosc.h \
	metal_header/sifive_fe310_g000_pll.h \
	metal_header/sifive_fe310_g000_prci.h \
	metal_header/sifive_fu540_c000_l2.h \
	metal_header/sifive_global_external_interrupts0.h \
	metal_header/sifive_gpio0.h \
	metal_header/sifive_gpio_button.h \
	metal_header/sifive_gpio_led.h \
	metal_header/sifive_gpio_switch.h \
	metal_header/sifive_local_external_interrupts0.h \
	metal_header/sifive_spi0.h \
	metal_header/sifive_test0.h \
	metal_header/sifive_uart0.h \
	metal_header/sifive_lpdma0.h \
	metal_header/stdout_path.h \
	metal_header/template_device.h

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
	bare_header/sifive_clic0.h \
	bare_header/sifive_fe310_g000_hfrosc.h \
	bare_header/sifive_fe310_g000_hfxosc.h \
	bare_header/sifive_fe310_g000_prci.h \
	bare_header/sifive_fe310_g000_pll.h \
	bare_header/sifive_fu540_c000_l2.h \
	bare_header/sifive_global_external_interrupts0.h \
	bare_header/sifive_gpio0.h \
	bare_header/sifive_gpio_buttons.h \
	bare_header/sifive_gpio_leds.h \
	bare_header/sifive_gpio_switches.h \
	bare_header/sifive_i2c0.h \
	bare_header/sifive_local_external_interrupts0.h \
	bare_header/sifive_pwm0.h \
	bare_header/sifive_spi0.h \
	bare_header/sifive_test0.h \
	bare_header/sifive_uart0.h \
	bare_header/sifive_lpdma0.h

freedom_zephyrdtsfixup_generator_SOURCES = \
	freedom-zephyrdtsfixup-generator.c++ \
	fdt.c++

freedom_metal_specs_generator_SOURCES = \
	freedom-metal_specs-generator.c++ \
	fdt.c++

AM_TESTS_ENVIRONMENT = \
  export SOURCE_DIR=$(abspath $(srcdir)); \
  export WORK_DIR=$(abspath .);

TESTS       =
EXTRA_DIST  =
TESTS      += tests/e31-arty-ldscript.bash
TESTS      += tests/e31-arty-openocd.bash
TESTS      += tests/e31-arty-makeattributes.bash
TESTS      += tests/e31-arty-metal_header.bash
EXTRA_DIST += tests/e31-arty.dts
TESTS      += tests/e51-arty-ldscript.bash
TESTS      += tests/e51-arty-openocd.bash
TESTS      += tests/e51-arty-makeattributes.bash
TESTS      += tests/e51-arty-metal_header.bash
EXTRA_DIST += tests/e51-arty.dts
TESTS      += tests/qemu-sifive_e-32-ldscript.bash
TESTS      += tests/qemu-sifive_e-32-openocd.bash
TESTS      += tests/qemu-sifive_e-32-makeattributes.bash
TESTS      += tests/qemu-sifive_e-32-metal_header.bash
EXTRA_DIST += tests/qemu-sifive_e-32.dts
TESTS      += tests/hifive1-ldscript.bash
TESTS      += tests/hifive1-openocd.bash
TESTS      += tests/hifive1-makeattributes.bash
TESTS      += tests/hifive1-metal_header.bash
TESTS      += tests/hifive1-zephyrdtsfixup.bash
TESTS      += tests/hifive1-metal_specs.bash
EXTRA_DIST += tests/hifive1.dts
#TESTS      += tests/e31-eval-ldscript.bash
TESTS      += tests/e31-eval-openocd.bash
#TESTS      += tests/e31-eval-makeattributes.bash
TESTS      += tests/e31-eval-metal_header.bash
EXTRA_DIST += tests/e31-eval.dts
TESTS      += tests/multi-e31-arty-ldscript.bash
TESTS      += tests/multi-e31-arty-openocd.bash
TESTS      += tests/multi-e31-arty-makeattributes.bash
TESTS      += tests/multi-e31-arty-metal_header.bash
EXTRA_DIST += tests/multi-e31-arty.dts
TESTS      += tests/e31-bigmem-ldscript.bash
TESTS      += tests/e31-bigmem-openocd.bash
TESTS      += tests/e31-bigmem-makeattributes.bash
TESTS      += tests/e31-bigmem-metal_header.bash
EXTRA_DIST += tests/e31-bigmem.dts
#TESTS      += tests/e34-eval-ldscript.bash
TESTS      += tests/e34-eval-openocd.bash
#TESTS      += tests/e34-eval-makeattributes.bash
TESTS      += tests/e34-eval-metal_header.bash
TESTS      += tests/e34-eval-metal_specs.bash
EXTRA_DIST += tests/e34-eval.dts
TESTS      += tests/e31-memory-ldscript.bash
TESTS      += tests/e31-memory-openocd.bash
#TESTS      += tests/e31-memory-makeattributes.bash
TESTS      += tests/e31-memory-metal_header.bash
EXTRA_DIST += tests/e31-memory.dts
#TESTS      += tests/e31-ranges-ldscript.bash
TESTS      += tests/e31-ranges-openocd.bash
TESTS      += tests/e31-ranges-makeattributes.bash
TESTS      += tests/e31-ranges-metal_header.bash
EXTRA_DIST += tests/e31-ranges.dts

EXTRA_DIST += $(TESTS)

EXTRA_DIST += README.md
EXTRA_DIST += fdt.h++
EXTRA_DIST += libfdt.h++
EXTRA_DIST += multilib.h++
