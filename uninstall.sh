#!/bin/bash

set -euo pipefail

# Copyright:
#     - Kai Krakow from xpadneo (https://github.com/atar-axis/xpadneo/)
#     - Max Görner
# License: GPL v3.0


cd "$(dirname "$0")" || exit 1
source "scripts-lib/verbose.sh"
source "scripts-lib/installer.sh"

echo "* unloading current driver module"
modprobe -r "$MODULE_NAME" || true

echo "* looking for registered instances"
echo "found ${#INSTALLED[@]} registered instance(s) on your system"

for instance in "${INSTALLED[@]}"
do
    echo "* $instance"

    set -e

    echo "  * uninstalling and removing $instance from DKMS"
    dkms remove "${V[*]}" "$MODULE_NAME/${instance}" --all

    echo "  * removing $instance folder from /usr/src"
    rm --recursive "/usr/src/$MODULE_NAME-$instance/"
done
