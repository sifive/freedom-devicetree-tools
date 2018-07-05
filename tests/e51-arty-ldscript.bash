set -e

tempdir="$(mktemp -d)"
trap "rm -rf $tempdir" EXIT

cd "$tempdir"

dtc $SOURCE_DIR/tests/e51-arty.dts -o e51-arty.dtb -O dtb
$WORK_DIR/freedom-ldscript-generator -d e51-arty.dtb -l e51-arty.lds
