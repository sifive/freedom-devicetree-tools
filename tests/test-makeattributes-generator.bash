#!/usr/bin/env bash

set -e

unset TARGET
unset TARGET_TYPE
TARGET=$1
TARGET_TYPE=$2

tempdir="$(mktemp -d)"
trap "rm -rf $tempdir" EXIT

cd "$tempdir"

dtc -I dts -O dtb -o design.dtb $SOURCE_DIR/tests/$TARGET/design.dts
$WORK_DIR/freedom-makeattributes-generator -d design.dtb -b $TARGET_TYPE -o settings.mk
