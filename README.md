# Device Tree Tools for the Freedom SDK

This project contains a handful of tools that are designed to aid
embedded software developers to generate statically parameterized designs
from Freedom platform device trees.  Help for the various tools can be
found by passing the `--help` argument, to the tool once they have been
built.

If you're interested in using the tools then it's probably best to check
out the Freedom SDK, which uses these tools to parameterize the Freedom
METAL for the various targets that it can support.

As a quick overview, here are the tools that are installed as part of
this project:

* `freedom-ldscript-generator`: Generates linker scripts that are
  compatible with the Freedom METAL from a device tree.
* `freedom-makeattributes-generator`: Generates Makefile attributes
  (CFLAGS, LDFLAGS, etc) from a device tree.
* `freedom-mee_header-generator`: Generates a METAL header
  parameterization file, which allows the METAL to target the device
  described by the given device tree.
* `freedom-openocdcfg-generator`: Generates an OpenOCD configuration
  file from a device tree.
* `freedom-zephyrdtsfixup-generator`: Generates a dts.fixup configuration
  file from a device tree. This is a temporary requirement of the Zephyr
  RTOS project to support DTS-based driver configuration.

## How to Build

If you're just planning on using this project then you should use the
latest tarball release, which is always available from the [GitHub
releases
page](https://github.com/sifive/freedom-ldscript-generator/releases).
Builds are uploaded from Travis after they succeed.

### Ubuntu 16.04 LTS

The following packages are necessary to build from a tarball

* `build-essential`
* `libfdt-dev`
* `device-tree-compiler`

Additionally, the following packages are necessary to build from git
sources:

* `autoconf`
* `automake`
* `git`
