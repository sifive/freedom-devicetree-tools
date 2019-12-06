#!/usr/bin/env bash

set -e

unset TARGET
TARGET=$1

tempdir="$(mktemp -d)"
trap "rm -rf $tempdir" EXIT

cd "$tempdir"

dtc -I dts -O dtb -o design.dtb $SOURCE_DIR/tests/$TARGET/design.dts
$WORK_DIR/freedom-ldscript-generator -d design.dtb -l $TARGET.lds
$WORK_DIR/freedom-ldscript-generator -d design.dtb -l $TARGET.lds --scratchpad
$WORK_DIR/freedom-ldscript-generator -d design.dtb -l $TARGET.lds --ramrodata
