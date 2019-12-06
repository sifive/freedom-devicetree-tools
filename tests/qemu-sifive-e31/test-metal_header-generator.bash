#!/usr/bin/env bash

set -e

DIR=$(dirname $0)
TARGET=${DIR##*/}

$SOURCE_DIR/tests/test-metal_header-generator.bash $TARGET
