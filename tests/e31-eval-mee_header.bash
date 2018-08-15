set -e

tempdir="$(mktemp -d)"
trap "rm -rf $tempdir" EXIT

cd "$tempdir"

dtc $SOURCE_DIR/tests/e31-eval.dts -o e31-eval.dtb -O dtb
$WORK_DIR/freedom-mee_header-generator -d e31-eval.dtb -o e31-eval.h

cat e31-eval.h
if [[ "$(grep "struct __mee_driver_sifive_test0" e31-eval.h | wc -l)" == 0 ]]
then
    echo "The E31 evaluation config has a test finisher that must be used, as otherwise the RTL test harness will take forever to run." >&2
    exit 1
fi