set -e

tempdir="$(mktemp -d)"
trap "rm -rf $tempdir" EXIT

cd "$tempdir"

dtc $SOURCE_DIR/tests/e31-memory.dts -o e31-memory.dtb -O dtb
$WORK_DIR/freedom-ldscript-generator -d e31-memory.dtb -l e31-memory.lds

cat e31-memory.lds
if [[ "$(grep ">ram" e31-memory.lds | wc -l)" == 0 ]]
then
    echo "The E31 eval config must load code into the memory" >&2
    exit 1
fi

if [[ "$(grep "ram (wxa!ri) : ORIGIN = 0x80000000, LENGTH = 0x20000000" e31-memory.lds | wc -l)" == 0 ]]
then
    echo "The E31 eval config must load code into the test RAM on memory" >&2
    exit 1
fi
