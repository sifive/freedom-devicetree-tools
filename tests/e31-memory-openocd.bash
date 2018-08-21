set -e

tempdir="$(mktemp -d)"
trap "rm -rf $tempdir" EXIT

cd "$tempdir"

dtc $SOURCE_DIR/tests/e31-memory.dts -o e31-memory.dtb -O dtb
$WORK_DIR/freedom-openocdcfg-generator -b arty -d e31-memory.dtb -o e31-memory-openocd.cfg
