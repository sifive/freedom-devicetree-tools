set -e

tempdir="$(mktemp -d)"
trap "rm -rf $tempdir" EXIT

cd "$tempdir"

dtc $SOURCE_DIR/tests/e34-eval.dts -o e34-eval.dtb -O dtb
$WORK_DIR/freedom-ldscript-generator -d e34-eval.dtb -l e34-eval.lds

cat e34-eval.lds

if [[ "$(grep "flash (rxai!w) : ORIGIN = 0x20000000, LENGTH = 0x2000" e34-eval.lds | wc -l)" == 0 ]]
then
    echo "The E34 eval config must load code into the test RAM next to the AHB periph port" >&2
    exit 1
fi

