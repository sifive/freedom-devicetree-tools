set -e

tempdir="$(mktemp -d)"
trap "rm -rf $tempdir" EXIT

cd "$tempdir"

dtc $SOURCE_DIR/tests/e31-ranges.dts -o e31-ranges.dtb -O dtb
$WORK_DIR/freedom-ldscript-generator -d e31-ranges.dtb -l e31-ranges.lds

cat e31-ranges.lds

if [[ "$(grep "flash (rxai!w) : ORIGIN = 0x20000000, LENGTH = 0x20000000" e31-ranges.lds | wc -l)" == 0 ]]
then
    echo "The E31 eval config must load code into the test RAM next to the AHB periph port" >&2
    exit 1
fi

