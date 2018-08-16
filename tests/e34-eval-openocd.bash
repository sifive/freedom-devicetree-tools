set -e

tempdir="$(mktemp -d)"
trap "rm -rf $tempdir" EXIT

cd "$tempdir"

dtc $SOURCE_DIR/tests/e34-eval.dts -o e34-eval.dtb -O dtb
$WORK_DIR/freedom-openocdcfg-generator -b arty -d e34-eval.dtb -o e34-eval-openocd.cfg
