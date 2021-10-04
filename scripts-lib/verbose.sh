#!/bin/bash

# Copyright:
#     - Kai Krakow from xpadneo (https://github.com/atar-axis/xpadneo/)
#     - Max Görner
# License: GPL v3.0

case "$1" in
    --verbose)
        echo "* verbose mode enabled"
        # shellcheck disable=SC2034
        V=("$1")
        set -x
        ;;
    "")
        ;;
    *)
        echo "usage: $0 [--verbose]"
        exit 1
esac
