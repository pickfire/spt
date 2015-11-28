# spt - simple pomodoro timer
# See LICENSE file for copyright and license details.

include config.mk

SRC = spt.c
OBJ = ${SRC:.c=.o}

all: options spt

options:
	@echo spt build options:
	@echo "CFLAGS  = ${CFLAGS}"
	@echo "LDFLAGS = ${LDFLAGS}"
	@echo "CC      = ${CC}"

.c.o:
	@echo CC $<
	@${CC} -c ${CFLAGS} $<

${OBJ}: config.h config.mk

config.h:
	@echo creating $@ from config.def.h
	@cp config.def.h $@

spt: ${OBJ}
	@echo CC -o $@
	@${CC} -o $@ ${OBJ} ${LDFLAGS}

clean:
	@echo cleaning
	@rm -f ${SRC:.c=} ${OBJ} spt-${VERSION}.tar.xz

dist: clean
	@echo creating dist tarball
	@mkdir -p spt-${VERSION}
	@cp -R LICENSE Makefile README config.mk config.def.h spt.info spt.1 ${SRC} spt-${VERSION}
	@tar -cf spt-${VERSION}.tar spt-${VERSION}
	@xz spt-${VERSION}.tar
	@rm -rf spt-${VERSION}

install: all
	@echo installing executable file to ${DESTDIR}${PREFIX}/bin
	@mkdir -p ${DESTDIR}${PREFIX}/bin
	@cp -f spt ${DESTDIR}${PREFIX}/bin
	@chmod 755 ${DESTDIR}${PREFIX}/bin/spt
	@echo installing manual page to ${DESTDIR}${MANPREFIX}/man1
	@mkdir -p ${DESTDIR}${MANPREFIX}/man1
	@sed "s/VERSION/${VERSION}/g" < spt.1 > ${DESTDIR}${MANPREFIX}/man1/spt.1
	@chmod 644 ${DESTDIR}${MANPREFIX}/man1/spt.1

uninstall:
	@echo removing executable file from ${DESTDIR}${PREFIX}/bin
	@rm -f ${DESTDIR}${PREFIX}/bin/spt
	@echo removing manual page from ${DESTDIR}${MANPREFIX}/man1
	@rm -f ${DESTDIR}${MANPREFIX}/man1/spt.1

.PHONY: all options clean dist install uninstall
