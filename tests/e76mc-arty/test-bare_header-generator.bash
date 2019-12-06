#!/usr/bin/env bash

set -e

DIR=$(dirname $0)
TARGET=${DIR##*/}

$SOURCE_DIR/tests/test-bare_header-generator.bash $TARGET
