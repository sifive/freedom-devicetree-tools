set -e

tempdir="$(mktemp -d)"
trap "rm -rf $tempdir" EXIT

cd "$tempdir"

dtc $SOURCE_DIR/tests/e34-eval.dts -o e34-eval.dtb -O dtb
$WORK_DIR/freedom-metal_specs-generator -d e34-eval.dtb -s e34-eval-build.env --prefix $(pwd)/p

cat e34-eval-build.env

if [[ "$(cat e34-eval-build.env | grep march=rv32imafdc | wc -l)" == 0 ]]
then
    echo "wrong march"
    exit 1
fi

if [[ "$(cat e34-eval-build.env | grep mabi=ilp32d | wc -l)" == 0 ]]
then
    echo "wrong mabi"
    exit 1
fi
