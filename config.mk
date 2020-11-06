# spt version
VERSION = 0.6

# Customize below to fit your system

# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

INCS = -I. -I/usr/include
LIBS = -L/usr/lib

# libnotify, comment if you don't want it
DEFS = -DNOTIFY
INCS+= `pkg-config --cflags libnotify`
LIBS+= `pkg-config --libs libnotify`

# flags
CPPFLAGS += -DVERSION=\"${VERSION}\" -D_POSIX_C_SOURCE=199309
CFLAGS += -g -std=c99 -pedantic -Wall -Os ${INCS} ${DEFS} ${CPPFLAGS}
LDFLAGS += -g ${LIBS}

# compiler and linker
CC ?= cc
