#!/bin/bash

# Copyright:
#     - Kai Krakow from xpadneo (https://github.com/atar-axis/xpadneo/)
#     - Max Görner
# License: GPL v3.0

if [ ${EUID} -ne 0 ]; then
	echo >&2 "ERROR: You most probably need superuser privileges to use this script, please run me via sudo!"
	exit 3
fi

# shellcheck disable=SC2034
GIT_ROOT=$(git rev-parse --show-toplevel 2>/dev/null || true)
PROJECT_NAME="rtl88x2bu"
export MODULE_NAME="88x2bu"

__version_lte() {
	[ "$1" = "$(echo -e "$1\n${2/-/.9999-}" | sort -V | head -n1)" ]
}

# shellcheck disable=SC2034
VERSION="5.8.7.1"

DKMS_BIN=$(type -p dkms)

# shellcheck disable=SC2086
: ${DKMS_BIN:?Please install dkms to continue}

get_dkms_versions_installed() {
	${DKMS_BIN} status | tr -s ':, ' ' ' | awk -- "\$1 == \"$PROJECT_NAME\" { print \$2 }" | sort -nu
}

get_upstream_version_latest() {
	curl -sI "https://github.com/atar-axis/xpadneo/releases/latest" | \
	awk -- 'BEGIN{IGNORECASE=1} /^location:/ { n = split($2, v, /\//); print v[n]; exit }' | \
	tr -d '[:space:]'
}

cat_dkms_make_log() {
	local last_error=$?
	if [ -n "${V[*]}" ]; then
		cat "/var/lib/dkms/$PROJECT_NAME/${VERSION}/build/make.log" || true
	fi
	exit ${last_error}
}

# shellcheck disable=SC2034
INSTALLED=(
	$(get_dkms_versions_installed)
)
