set -e

tempdir="$(mktemp -d)"
trap "rm -rf $tempdir" EXIT

cd "$tempdir"

dtc $SOURCE_DIR/tests/qemu-sifive_e-32.dts -o qemu-sifive_e-32.dtb -O dtb
$WORK_DIR/freedom-ldscript-generator -d qemu-sifive_e-32.dtb -l qemu-sifive_e-32.lds
