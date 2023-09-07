#!/bin/bash

set -euo pipefail

function main() {
	local COMMAND FIRST_ARG

	if [[ $# -eq 0 ]]; then
		print-global-usage
		exit 1
	fi

	FIRST_ARG="$1"
	shift
	case "${FIRST_ARG}" in
	install | list-kernels | remove)
		COMMAND="${FIRST_ARG}"
		;;
	-h | --help)
		print-global-usage
		exit 0
		;;
	*)
		print-global-usage
		exit 1
		;;
	esac

	ensure_root_permissions
}

function print-global-usage() {
	cat <<EOF | fmt
Synopsis: $0 <install|list-kernels|remove|-h|--help> [--all]

-h|--help: display this help and exit
install: install the driver for specified Kernels
list-kernels: list all installed Kernels on STDOUT
remove: remove the driver for specified Kernels

SPECFIYING KERNELS:

Kernels for which the driver should be installed can be specified on STDIN. In order to get a list of all installed Kernels one can use the command \`list-kernels\`. Thus, in order to install the driver for all current Kernels, one can write

    $0 list-kernels | $0 install

If no Kernel is specified, the default behaviour of DKMS will be used, which is to install the driver for the currently running Kernel.
EOF
}

function ensure_no_cli_args() {
    if [ $# -ne 0 ]
    then
        echo "No command line arguments accepted!" >&2
        exit 1
    fi
}

function ensure_root_permissions() {
    if ! sudo -v
    then
        echo "Root permissions required to deploy the driver!" >&2
        exit 1
    fi
}

function get_version() {
    sed -En 's/PACKAGE_VERSION="(.*)"/\1/p' dkms.conf
}

function deploy_driver() {
    VER=$(get_version)
    sudo rsync --delete --exclude=.git -rvhP ./ "/usr/src/rtl88x2bu-${VER}"
    for action in add build install
    do
      sudo dkms "${action}" -m rtl88x2bu -v "${VER}"
    done
    sudo modprobe 88x2bu
}

ensure_no_cli_args "$@"
ensure_root_permissions
deploy_driver
