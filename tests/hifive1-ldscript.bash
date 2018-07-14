set -e

tempdir="$(mktemp -d)"
trap "rm -rf $tempdir" EXIT

cd "$tempdir"

dtc $SOURCE_DIR/tests/hifive1.dts -o hifive1.dtb -O dtb
$WORK_DIR/freedom-ldscript-generator -d hifive1.dtb -l hifive1.lds
