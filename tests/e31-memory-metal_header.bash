set -e

tempdir="$(mktemp -d)"
trap "rm -rf $tempdir" EXIT

cd "$tempdir"

dtc $SOURCE_DIR/tests/e31-memory.dts -o e31-memory.dtb -O dtb
$WORK_DIR/freedom-metal_header-generator -d e31-memory.dtb -o e31-memory.h

cat e31-memory.h
if [[ "$(grep "struct __metal_driver_sifive_test0" e31-memory.h | wc -l)" == 0 ]]
then
    echo "The E31 evaluation config has a test finisher that must be used, as otherwise the RTL test harness will take forever to run." >&2
    exit 1
fi
