DESCRIPTION = "Client application send periodic messages to server"
LICENSE = "MIT"
LIC_FILES_CHKSUM = "file://${COREBASE}/meta/COPYING.MIT;md5=3da9cfbcb788c80a0384361b4de20420"

inherit update-rc.d

SRC_URI = " \
  file://client.c \
  file://client_service \
  file://client.conf \
  "

S = "${WORKDIR}"

do_compile() {
    ${CC} ${CFLAGS} ${LDFLAGS} client.c -o client
}

do_install() {
    # Hook the client to init services
    install -d ${D}${sysconfdir}/init.d
    install -m 0755 client_service ${D}${sysconfdir}/init.d/client_service

    # Install client binary to /usr/bin directory
    install -d ${D}${bindir}
    install -m 0755 client ${D}${bindir}

    # Move initial config file to /etc/ directory
    install -d ${D}${sysconfdir}/client
    install -m 0644 client.conf ${D}${sysconfdir}/client/client.conf
}

INITSCRIPT_NAME = "client_service"
INITSCRIPT_PARAMS = "start 99 1 2 3 4 5 . stop 20 0 1 6 ."
RDEPENDS_${PN} = "initscripts"
CONFFILES_${PN} += "${sysconfdir}/init.d/client_service"
