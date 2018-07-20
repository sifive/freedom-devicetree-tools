set -e

tempdir="$(mktemp -d)"
trap "rm -rf $tempdir" EXIT

cd "$tempdir"

cat $SOURCE_DIR/tests/multi-e31-arty.dts

dtc $SOURCE_DIR/tests/multi-e31-arty.dts -o multi-e31-arty.dtb -O dtb
$WORK_DIR/freedom-ldscript-generator -d multi-e31-arty.dtb -l multi-e31-arty.lds

cat multi-e31-arty.lds

if [[ "$(grep "ram (wxa!ri) : ORIGIN" multi-e31-arty.lds | wc -l)" != 1 ]]
then
    echo "multiple RAM definitons"
    exit 1
fi
