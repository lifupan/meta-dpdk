#
# Copyright (C) 2013-2014 Wind River Systems, Inc.
#
require dpdk.inc

PV = "16.04"

DPDK_BASE_VERSION = "${PV}"

S= "${WORKDIR}/git"

SRC_URI += "git://dpdk.org/dpdk \
            file://0001-dpdk-stop-reloading-gcc-variables_2.0.patch \
            file://0002-tools-dpdk_nic_bind.py-remove-call-to-lower-case-for.patch \
            file://0003-Calling-netif_rx-from-memnic-thread-may-cause-deadlo.patch \
	    file://0004-Memzone-now-support-64-char-names-isntead-of-32.patch \
	    file://0005-lpm-iterate-through-the-routes.patch \
	    file://0006-lpm-rte_lpm6iterate-iterate-through-the-routes.patch \
	   "

SRCREV = "b3b9719f18ee83773c6ed7adda300c5ac63c37e9"
