#!/usr/bin/env bash

set -e

unset TARGET
TARGET=$1

tempdir="$(mktemp -d)"
trap "rm -rf $tempdir" EXIT

cd "$tempdir"

dtc -I dts -O dtb -o design.dtb $SOURCE_DIR/tests/$TARGET/design.dts
$WORK_DIR/freedom-openocdcfg-generator -d design.dtb -o openocd.cfg
