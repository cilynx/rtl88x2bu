#! /bin/bash

VER=$(sed -n 's/\PACKAGE_VERSION="\(.*\)"/\1/p' dkms.conf)
DRV_NAME=rtl88x2bu

sudo rsync -rvhP ./ /usr/src/rtl88x2bu-${VER}
dkms add -m ${DRV_NAME} -v ${VER}
dkms build -m ${DRV_NAME} -v ${VER}
dkms install -m ${DRV_NAME} -v ${VER}
sudo modprobe 88x2bu
