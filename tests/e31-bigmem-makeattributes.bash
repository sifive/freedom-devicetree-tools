set -e

tempdir="$(mktemp -d)"
trap "rm -rf $tempdir" EXIT

cd "$tempdir"

dtc $SOURCE_DIR/tests/e31-bigmem.dts -o e31-bigmem.dtb -O dtb
$WORK_DIR/freedom-makeattributes-generator -d e31-bigmem.dtb -o e31-bigmem-build.env
