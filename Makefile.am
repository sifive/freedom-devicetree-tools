# Copyright 2018 SiFive, Inc
# SPDX-License-Identifier: Apache-2.0

# A linker script generator for SiFive's Freedom platform.
bin_PROGRAMS =  freedom-ldscript-generator \
		freedom-openocdcfg-generator \
		freedom-makeattributes-generator \
		freedom-metal_header-generator \
		freedom-bare_header-generator \
		freedom-metal_specs-generator


freedom_ldscript_generator_CPPFLAGS = -I$(srcdir) -I$(srcdir)/linker_script

freedom_ldscript_generator_SOURCES = \
	fdt.c++ \
	linker_script/freedom-ldscript-generator.c++ \
	linker_script/layouts/default_layout.c++ \
	linker_script/layouts/default_layout.h \
	linker_script/layouts/ramrodata_layout.c++ \
	linker_script/layouts/ramrodata_layout.h \
	linker_script/layouts/scratchpad_layout.c++ \
	linker_script/layouts/scratchpad_layout.h \
	linker_script/sections/constants_group.c++ \
	linker_script/sections/constants_group.h \
	linker_script/sections/ctors_group.c++ \
	linker_script/sections/ctors_group.h \
	linker_script/sections/data_group.c++ \
	linker_script/sections/data_group.h \
	linker_script/sections/init_text_group.c++ \
	linker_script/sections/init_text_group.h \
	linker_script/sections/itim_text_group.c++ \
	linker_script/sections/itim_text_group.h \
	linker_script/sections/itim_group.c++ \
	linker_script/sections/itim_group.h \
	linker_script/sections/rodata_group.c++ \
	linker_script/sections/rodata_group.h \
	linker_script/sections/text_group.c++ \
	linker_script/sections/text_group.h \
	linker_script/sections/uninit_group.c++ \
	linker_script/sections/uninit_group.h \
	linker_script/strategies/chosen_strategy.c++ \
	linker_script/strategies/chosen_strategy.h \
	linker_script/strategies/default_bullet_arty.c++ \
	linker_script/strategies/default_bullet_arty.h \
	linker_script/strategies/default_bullet_strategy.c++ \
	linker_script/strategies/default_bullet_strategy.h \
	linker_script/strategies/default_e20_arty_strategy.c++ \
	linker_script/strategies/default_e20_arty_strategy.h \
	linker_script/strategies/default_e20_strategy.c++ \
	linker_script/strategies/default_e20_strategy.h \
	linker_script/strategies/default_e21_arty_strategy.c++ \
	linker_script/strategies/default_e21_arty_strategy.h \
	linker_script/strategies/default_e21_strategy.c++ \
	linker_script/strategies/default_e21_strategy.h \
	linker_script/strategies/default_rocket_arty.c++ \
	linker_script/strategies/default_rocket_arty.h \
	linker_script/strategies/default_rocket_strategy.c++ \
	linker_script/strategies/default_rocket_strategy.h \
	linker_script/strategies/memory_only_strategy.c++ \
	linker_script/strategies/memory_only_strategy.h \
	linker_script/linker_script.c++ \
	linker_script/linker_script.h \
	linker_script/map_strategy.c++ \
	linker_script/map_strategy.h \
	linker_script/memory.c++ \
	linker_script/memory.h \
	linker_script/phdr.c++ \
	linker_script/phdr.h \
	linker_script/section.c++ \
	linker_script/section.h \
	linker_script/section_group.c++ \
	linker_script/section_group.h \
	ranges.c++ \
	ranges.h

freedom_openocdcfg_generator_SOURCES = \
	freedom-openocdcfg-generator.c++ \
	fdt.c++

freedom_makeattributes_generator_SOURCES = \
	freedom-makeattributes-generator.c++ \
	fdt.c++

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
	metal_header/sifive_fu540_c000_l2.c++ \
	metal_header/sifive_fu540_c000_l2.h \
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
	bare_header/sifive_ccache0.h \
	bare_header/sifive_clic0.h \
	bare_header/sifive_fe310_g000_hfrosc.h \
	bare_header/sifive_fe310_g000_hfxosc.h \
	bare_header/sifive_fe310_g000_lfrosc.h \
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
	bare_header/sifive_rtc0.h \
	bare_header/sifive_spi0.h \
	bare_header/sifive_test0.h \
	bare_header/sifive_trace.h \
	bare_header/sifive_uart0.h \
	bare_header/sifive_wdog0.h \
	bare_header/ucb_htif0.h \
	ranges.c++ \
	ranges.h

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
