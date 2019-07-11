# Copyright 2018 SiFive, Inc
# SPDX-License-Identifier: Apache-2.0

# The name, version, and maintainer of this package
AC_INIT([freedom-devicetree-tools], [m4_esyscmd_s([./scripts/git-version])], [bsou@sifive.com])

AC_CONFIG_MACRO_DIR([m4])

# Initializes automake, disabling maintainer mode
AM_INIT_AUTOMAKE([foreign subdir-objects tar-ustar])
AM_MAINTAINER_MODE([disable])

# Looks up the key toolchain programs that are used when building libsifive,
# such as the C compiler.
AC_PROG_CXX

# This code requires C++ 11
AX_CXX_COMPILE_STDCXX_11([noext], [mandatory])

# This depends on the device tree compiler as well as the associated device
# tree parsing libraries.
AC_CHECK_HEADER([libfdt.h])
AC_SEARCH_LIBS([fdt_getprop], [fdt])
AC_CHECK_PROG(HAVE_DTC, dtc, [yes], [no])
if test "$HAVE_DTC" != "yes"
then
    AC_MSG_FAILURE([The dtc program cannot be found.])
fi

# Generates the remainder of the build system.
AC_CONFIG_FILES([Makefile])
AC_OUTPUT
