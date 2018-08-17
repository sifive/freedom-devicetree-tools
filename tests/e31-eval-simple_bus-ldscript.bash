set -e

tempdir="$(mktemp -d)"
trap "rm -rf $tempdir" EXIT

cd "$tempdir"

dtc $SOURCE_DIR/tests/e31-eval-simple_bus.dts -o e31-eval-simple_bus.dtb -O dtb
$WORK_DIR/freedom-ldscript-generator -d e31-eval-simple_bus.dtb -l e31-eval-simple_bus.lds

cat e31-eval-simple_bus.lds
if [[ "$(grep ">ram" e31-eval-simple_bus.lds | wc -l)" == 0 ]]
then
    echo "The E31 eval-simple_bus config must load code into the ahb-periph-port" >&2
    exit 1
fi

if [[ "$(grep "ram (wxa!ri) : ORIGIN = 0x20000000, LENGTH = 0x2000" e31-eval-simple_bus.lds | wc -l)" == 0 ]]
then
    echo "The E31 eval-simple_bus config must load code into the test RAM next to the AHB periph port" >&2
    exit 1
fi

if [[ "$(grep ">flash" e31-eval-simple_bus.lds | wc -l)" != 0 ]]
then
    echo "The E31 eval-simple_bus config can't reference a SPI flash as there isn't one" >&2
    exit 1
fi

