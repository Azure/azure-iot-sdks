DESCRIPTION = ""
LICENSE = "MIT"

GITROOT = "${HOME}/azure-iot-sdks"
SRC_URI = "\
    file://${GITROOT}/c/ \
    file://iotdm_edison_sample.service \
"
S = "${WORKDIR}${GITROOT}/c/"

LIC_FILES_CHKSUM = "file://${GITROOT}/LICENSE;md5=4283671594edec4c13aeb073c219237a"
 
#what component does this recipe provide?
PROVIDES = "iotdm-edison-sample"
 
#what does this recipe depend on?
DEPENDS = "glib-2.0 curl openssl util-linux"
 
#parameters to cmake
EXTRA_OECMAKE = "-Drun_valgrind:BOOL=OFF -DcompileOption_C:STRING=OFF -Drun_e2e_tests:BOOL=OFF -Drun_longhaul_tests=OFF -Duse_amqp:BOOL=OFF -Duse_http:BOOL=OFF -Duse_mqtt:BOOL=OFF -Dskip_unittests:BOOL=ON"
 
SYSTEMD_SERVICE_${PN} = "iotdm_edison_sample.service"
 
FILES_${PN} = "${systemd_unitdir}/system/*"
FILES_${PN} += "${bindir}"
 
FILES_SOLIBSDEV = ""
 
inherit pkgconfig cmake systemd

do_fetch_append() {
    print "HOME=" + d.getVar("HOME",True);
    print "S=" + d.getVar("S",True);
    print "GITROOT=" + d.getVar("GITROOT",True)
    print "SRC_URI=[" + d.getVar("SRC_URI",True) + "]"
}

do_install_append() {
    install -d ${D}${systemd_unitdir}/system

    install -c -m 0644 ${WORKDIR}/iotdm_edison_sample.service ${D}${systemd_unitdir}/system

    install -d ${D}${bindir}
    install -c -m 0755 ${WORKDIR}/build/iotdm_client/samples/iotdm_edison_sample/iotdm_edison_sample ${D}${bindir}
}

