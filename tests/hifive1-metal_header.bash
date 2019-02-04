set -ex

tempdir="$(mktemp -d)"
trap "rm -rf $tempdir" EXIT

cd "$tempdir"

dtc $SOURCE_DIR/tests/hifive1.dts -o hifive1.dtb -O dtb
$WORK_DIR/freedom-metal_header-generator -d hifive1.dtb -o hifive1.h

cat hifive1.h

if [[ "$(grep ".clock = NULL," hifive1.h | wc -l)" != 0 ]]
then
    echo "There is an unlinked clock in the header" >&2
    exit 1
fi
