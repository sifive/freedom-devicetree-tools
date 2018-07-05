# Copyright 2018 SiFive, Inc
# SPDX-License-Identifier: Apache-2.0

# A linker script generator for SiFive's Freedom platform.
bin_PROGRAMS =  freedom-ldscript-generator \
		freedom_openocdcfg_generator \
		freedom_makeattributes_generator

freedom_ldscript_generator_SOURCES = \
	freedom-ldscript-generator.c++

freedom_openocdcfg_generator_SOURCES = \
	freedom-openocdcfg-generator.c++

freedom_makeattributes_generator_SOURCES = \
	freedom-makeattributes-generator.c++

AM_TESTS_ENVIRONMENT = \
  export SOURCE_DIR=$(abspath $(srcdir)); \
  export WORK_DIR=$(abspath .);

TESTS       =
EXTRA_DIST  =
TESTS      += tests/e31-arty-ldscript.bash
EXTRA_DIST += tests/e31-arty.dts
TESTS      += tests/e51-arty-ldscript.bash
EXTRA_DIST += tests/e51-arty.dts

EXTRA_DIST += $(TESTS)
