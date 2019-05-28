set -e

tempdir="$(mktemp -d)"
trap "rm -rf $tempdir" EXIT

cd "$tempdir"

dtc $SOURCE_DIR/tests/e34-eval.dts -o e34-eval.dtb -O dtb
$WORK_DIR/freedom-makeattributes-generator -d e34-eval.dtb -o e34-eval-build.env

if [[ "$(cat e34-eval-build.env | grep RISCV_ARCH=rv32imafdc | wc -l)" == 0 ]]
then
    echo "wrong march"
    exit 1
fi

if [[ "$(cat e34-eval-build.env | grep RISCV_ABI=ilp32d | wc -l)" == 0 ]]
then
    echo "wrong mabi"
    exit 1
fi
