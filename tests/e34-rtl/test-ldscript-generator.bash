#!/usr/bin/env bash

set -e

DIR=$(dirname $0)
TARGET=${DIR##*/}

$SOURCE_DIR/tests/test-ldscript-generator.bash $TARGET
