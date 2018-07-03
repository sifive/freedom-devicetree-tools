# freedom-ldscript-generator

This project contains a linker script generator that targets SiFive's
Freedom platform.  It consumes a flattened device tree and produces a
linker script suitable for mapping programs to that target machine.
More information can be seen by running `./freedom-ldscript-generator
--help`.

## How to Build

If you're just planning on using this project then you should use the
latest tarball release, which is always availiable from the [GitHub
releases
page](https://github.com/sifive/freedom-ldscript-generator/releases).
Builds are uploaded from Travis after they succeed.

### Ubuntu 16.04 LTS

The following packages are necessary to build from a tarball

* `build-essential`
* `libfdt-dev`

Additionally, the following packages are necessary to build from git
sources:

* `autoconf`
* `automake`

If you're planning on running the test cases, then you'll also need the
following programs

* `device-tree-compiler`
