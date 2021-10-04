#!/bin/bash

set -euo pipefail

# Authors:
#     - Kai Krakow from xpadneo (https://github.com/atar-axis/xpadneo/)
#     - Max Görner
# License: GPL v3.0


cd "$(dirname "$0")" || exit 1
source "scripts-lib/verbose.sh"
source "scripts-lib/installer.sh"

if [[ -z "${INSTALLED[*]}" ]]; then

    set -e

    echo "* creating dkms.conf"
    sed 's/"@DO_NOT_CHANGE@"/"'"${VERSION}"'"/g' scripts-lib/dkms.conf.in >scripts-lib/dkms.conf

    echo "* installing module (using DKMS)"
    dkms install "${V[*]}" "hid-xpadneo" --force || cat_dkms_make_log

else

    echo "already installed!"

fi
