set -e

tempdir="$(mktemp -d)"
trap "rm -rf $tempdir" EXIT

cd "$tempdir"

dtc $SOURCE_DIR/tests/multi-e31-arty.dts -o multi-e31-arty.dtb -O dtb
$WORK_DIR/freedom-metal_header-generator -d multi-e31-arty.dtb -o multi-e31-arty-build.env
