#!/bin/bash

# Copyright:
#     - Kai Krakow from xpadneo (https://github.com/atar-axis/xpadneo/)
#     - Max Görner
# License: GPL v3.0

function exit_with_errmsg() {
    echo "usage: $0 [--verbose]" >&2
    exit 1
}

if [[ $# -eq 0 ]]
then
    V=()
elif [[ $# -eq 1 ]]
then
    case "${1}" in
        --verbose)
            echo "* verbose mode enabled"
            # shellcheck disable=SC2034
            V=("$1")
            set -x
            ;;
        "")
            ;;
        *)
            exit_with_errmsg
    esac
else
    exit_with_errmsg
fi
