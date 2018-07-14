set -e

tempdir="$(mktemp -d)"
trap "rm -rf $tempdir" EXIT

cd "$tempdir"

dtc $SOURCE_DIR/tests/hifive1.dts -o hifive1.dtb -O dtb
$WORK_DIR/freedom-openocdcfg-generator -b arty -d hifive1.dtb -o hifive1-openocd.cfg
