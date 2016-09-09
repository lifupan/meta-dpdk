#
# Copyright (C) 2016-2017 Wind River Systems, Inc.
#
SUMMARY = "Intel Data Plane Development Kit"
DESCRIPTION = "Intel Data Plane Development Kit (DPDK) presents a \
solution for fast-path network processing. DPDK allows you to make \
use of packet processing features available on the latest Intel processors."
LICENSE = "BSD & GPLv2 & LGPLv2"
LIC_FILES_CHKSUM = "file://${COMMON_LICENSE_DIR}/BSD;md5=3775480a712fc46a69647678acb234cb \
                    file://LICENSE.GPL;md5=751419260aa954499f7abaabaa882bbe \
                    file://LICENSE.LGPL;md5=4fbd65380cdd255951079008b364516c"

COMPATIBLE_HOST = '(x86_64.*|i.86.*)-linux'

RDEPENDS_${PN} += "kmod procps"
DEPENDS_${PN} += "libvirt"

DPDK_ARCH = "${TARGET_ARCH}"
DPDK_ARCH_i586 = "i686"
DPDK_TARGET ?= "${DPDK_ARCH}-native-linuxapp-gcc"
DPDK_TARGET_MACHINE ?= "default"

S = "${WORKDIR}/DPDK-${DPDK_BASE_VERSION}"
B = "${S}/${DPDK_TARGET}"

inherit module

EXTRA_OEMAKE += '\
                 TARGET_LDFLAGS= \
                 TARGET_CFLAGS= \
                 CROSS=${TARGET_PREFIX} \
                 RTE_KERNELDIR="${STAGING_KERNEL_BUILDDIR}" \
                 RTE_SDK="${S}" \
                 RTE_TARGET=${DPDK_TARGET} '

PACKAGES += "${PN}-examples"

FILES_${PN} += "\
                /opt/dpdk/app \
                /opt/dpdk/bin \
                /opt/dpdk/sbin \
                /opt/dpdk/tools \
               "

FILES_${PN}-examples += "/opt/dpdk/examples"

FILES_${PN}-dev += "\
                    /opt/dpdk/share \
                    /opt/dpdk/include \
                   "

FILES_${PN}-staticdev += "/opt/dpdk/lib"

do_configure() {
	oe_runmake -C ${S} O=${B} T=${DPDK_TARGET} config

	# This line changes default compiler flags from native to proper value
	sed -i 's/"native"/"${DPDK_TARGET_MACHINE}"/' ${B}/.config
}

do_compile() {
	oe_runmake
}

do_install() {
	oe_runmake install  DESTDIR=${D}/opt/dpdk prefix="" bindir=/bin sbindir=/sbin \
	includedir=/include libdir=/lib datadir=/share

	rm -fr ${D}/opt/dpdk/lib/modules
	install -m 0644 -D ${B}/kmod/igb_uio.ko \
	    ${D}/lib/modules/${KERNEL_VERSION}/kernel/drivers/uio/igb_uio.ko

	install -m 0644 -D ${B}/kmod/rte_kni.ko \
	    ${D}/lib/modules/${KERNEL_VERSION}/kernel/drivers/net/rte_kni.ko

	install -d ${D}/opt/dpdk/app/
	install -m 0755 ${B}/app/test ${D}/opt/dpdk/app/
	install -m 0755 ${B}/app/testpmd ${D}/opt/dpdk/app/

	install -d ${D}/opt/dpdk/tools
	install -m 0755 ${D}/opt/dpdk/share/tools/*.py ${D}/opt/dpdk/tools/

	chown -R root:root ${D}/opt/dpdk
}

do_compile_append() {
	oe_runmake -C ${S} examples T=${DPDK_TARGET} O=${B}/examples
}

do_install_append() {
	install -d ${D}/opt/dpdk/examples/
	dirs=$(find ${B}/examples/ -name "${DPDK_TARGET}" -type d)
	for dir in ${dirs}
	do
		if [ -d ${dir}/app ]
		then
			install -m 0755 ${dir}/app/* ${D}/opt/dpdk/examples/
		fi
	done
	rm -f ${D}/opt/dpdk/examples/*.map
}

pkg_postinst_${PN}() {
        if [ "x$D" != "x" ]; then
                exit 1
        fi

	echo "" >> /etc/sysctl.conf
	echo "# Controls the number of hugepages allocated" >> /etc/sysctl.conf
	echo "vm.nr_hugepages=64" >> /etc/sysctl.conf

	echo "" >> /etc/fstab
	echo "# Allow userspace applications to request huge pages via mmap system calls" >> /etc/fstab
	echo "huge /mnt/huge hugetlbfs defaults 0 0" >> /etc/fstab

	if [ ! -d /mnt/huge ]; then
		mkdir /mnt/huge
	fi
}

dpdk_sysroot_preprocess() {
	sysroot_stage_dir ${D}/opt/dpdk/share/mk/ ${SYSROOT_DESTDIR}/opt/dpdk/mk/
	sysroot_stage_dir ${D}/opt/dpdk/share/scripts/ ${SYSROOT_DESTDIR}/opt/dpdk/scripts/
	sysroot_stage_dir ${D}/opt/dpdk/lib/ ${SYSROOT_DESTDIR}/opt/dpdk/${DPDK_TARGET}/lib/
	sysroot_stage_dir ${D}/opt/dpdk/include/ ${SYSROOT_DESTDIR}/opt/dpdk/${DPDK_TARGET}/include/
	sysroot_stage_dir ${D}/opt/dpdk/share/${DPDK_TARGET}/ ${SYSROOT_DESTDIR}/opt/dpdk/${DPDK_TARGET}/
}
SYSROOT_PREPROCESS_FUNCS += "dpdk_sysroot_preprocess"
