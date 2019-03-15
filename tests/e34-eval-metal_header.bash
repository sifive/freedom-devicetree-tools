set -e

tempdir="$(mktemp -d)"
trap "rm -rf $tempdir" EXIT

cd "$tempdir"

dtc $SOURCE_DIR/tests/e34-eval.dts -o e34-eval.dtb -O dtb
$WORK_DIR/freedom-metal_header-generator -d e34-eval.dtb -o e34-eval.h

cat e34-eval.h
if [[ "$(grep "struct __metal_driver_sifive_test0" e34-eval.h | wc -l)" == 0 ]]
then
    echo "The E31 evaluation config has a test finisher that must be used, as otherwise the RTL test harness will take forever to run." >&2
    exit 1
fi
