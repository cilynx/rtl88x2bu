#!/bin/bash

set -euo pipefail

VER="$(sed -En 's/^PACKAGE_VERSION="(.*)"/\1/p' dkms.conf)"
DRV_NAME=rtl88x2bu

if ! sudo -v
then
    echo "Root privileges required to run this script!" >&2
    exit 1
fi

sudo rsync -rh ./ /usr/src/"${DRV_NAME}"-"${VER}"
sudo dkms add -m "${DRV_NAME}" -v "${VER}"
sudo dkms build -m "${DRV_NAME}" -v "${VER}"
sudo dkms install -m "${DRV_NAME}" -v "${VER}"
sudo modprobe 88x2bu
