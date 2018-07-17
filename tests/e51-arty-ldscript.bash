set -e

tempdir="$(mktemp -d)"
trap "rm -rf $tempdir" EXIT

cd "$tempdir"

cat $SOURCE_DIR/tests/e51-arty.dts

dtc $SOURCE_DIR/tests/e51-arty.dts -o e51-arty.dtb -O dtb
$WORK_DIR/freedom-ldscript-generator -d e51-arty.dtb -l e51-arty.lds

cat e51-arty.lds

if [[ "$(grep "ram (wxa!ri) : ORIGIN = 0x80000000, LENGTH = 0x10000" e51-arty.lds | wc -l)" == 0 ]]
then
    echo "Incorrect RAM address region" >&2
    exit 1
fi
