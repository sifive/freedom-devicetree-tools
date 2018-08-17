set -e

tempdir="$(mktemp -d)"
trap "rm -rf $tempdir" EXIT

cd "$tempdir"

dtc $SOURCE_DIR/tests/e31-eval-simple_bus.dts -o e31-eval-simple_bus.dtb -O dtb
$WORK_DIR/freedom-mee_header-generator -d e31-eval-simple_bus.dtb -o e31-eval-simple_bus.h

cat e31-eval-simple_bus.h
if [[ "$(grep "struct __mee_driver_sifive_test0" e31-eval-simple_bus.h | wc -l)" == 0 ]]
then
    echo "The E31 eval-simple_busuation config has a test finisher that must be used, as otherwise the RTL test harness will take forever to run." >&2
    exit 1
fi
