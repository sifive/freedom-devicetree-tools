set -ex

tempdir="$(mktemp -d)"
trap "rm -rf $tempdir" EXIT

cd "$tempdir"

dtc $SOURCE_DIR/tests/hifive1.dts -o hifive1.dtb -O dtb
$WORK_DIR/freedom-metal_specs-generator -d hifive1.dtb --specs hifive1.specs --prefix $(pwd)

cat hifive1.specs
