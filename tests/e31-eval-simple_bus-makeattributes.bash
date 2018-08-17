set -e

tempdir="$(mktemp -d)"
trap "rm -rf $tempdir" EXIT

cd "$tempdir"

dtc $SOURCE_DIR/tests/e31-eval-simple_bus.dts -o e31-eval-simple_bus.dtb -O dtb
$WORK_DIR/freedom-makeattributes-generator -d e31-eval-simple_bus.dtb -o e31-eval-simple_bus-build.env
