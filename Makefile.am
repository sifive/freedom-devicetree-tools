# Copyright 2018 SiFive, Inc
# SPDX-License-Identifier: Apache-2.0

# A linker script generator for SiFive's Freedom platform.
bin_PROGRAMS =  freedom-ldscript-generator \
		freedom-openocdcfg-generator \
		freedom-makeattributes-generator \
		freedom-mee_header-generator

freedom_ldscript_generator_SOURCES = \
	freedom-ldscript-generator.c++

freedom_openocdcfg_generator_SOURCES = \
	freedom-openocdcfg-generator.c++

freedom_makeattributes_generator_SOURCES = \
	freedom-makeattributes-generator.c++

freedom_mee_header_generator_SOURCES = \
	freedom-mee_header-generator.c++

AM_TESTS_ENVIRONMENT = \
  export SOURCE_DIR=$(abspath $(srcdir)); \
  export WORK_DIR=$(abspath .);

TESTS       =
EXTRA_DIST  =
TESTS      += tests/e31-arty-ldscript.bash
TESTS      += tests/e31-arty-openocd.bash
TESTS      += tests/e31-arty-makeattributes.bash
TESTS      += tests/e31-arty-mee_header.bash
EXTRA_DIST += tests/e31-arty.dts
TESTS      += tests/e51-arty-ldscript.bash
TESTS      += tests/e51-arty-openocd.bash
TESTS      += tests/e51-arty-makeattributes.bash
TESTS      += tests/e51-arty-mee_header.bash
EXTRA_DIST += tests/e51-arty.dts
TESTS      += tests/qemu-sifive_e-32-ldscript.bash
TESTS      += tests/qemu-sifive_e-32-openocd.bash
TESTS      += tests/qemu-sifive_e-32-makeattributes.bash
TESTS      += tests/qemu-sifive_e-32-mee_header.bash
EXTRA_DIST += tests/qemu-sifive_e-32.dts

EXTRA_DIST += $(TESTS)

EXTRA_DIST += README.md
