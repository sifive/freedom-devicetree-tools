#!/usr/bin/env bash

set -euo pipefail

INSTALL_PATH="install-path"

wake -vx "installFreedomDevicetreeTools \"${INSTALL_PATH}\""

>&2 echo "$0: Checking for ${INSTALL_PATH}"
if [ ! -d ${INSTALL_PATH} ] ; then
        >&2 echo "$0: ERROR Failed to produce ${INSTALL_PATH}"
        exit 1
fi

>&2 echo "$0: Checking for non-empty ${INSTALL_PATH}"
if [ ! -f ${INSTALL_PATH}/freedom-devicetree-tools/README.md ] ; then
        >&2 echo "$0: ERROR ${INSTALL_PATH} has bad contents"
        exit 2
fi

cd ${INSTALL_PATH}/freedom-devicetree-tools && ./configure && make -j$(nproc)
