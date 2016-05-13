# spt version
VERSION = 0.1

# Customize below to fit your system

# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

# libnotify - comment if you don't want it (config.h too)
INCS = -I. -I/usr/include `pkg-config --cflags libnotify`
LIBS = -L/usr/lib `pkg-config --libs libnotify`
DEFS = -DNOTIFY

# flags
CPPFLAGS = -DVERSION=\"${VERSION}\"
CFLAGS += -g -std=c99 -pedantic -Wall -Os ${INCS} ${DEFS} ${CPPFLAGS}
LDFLAGS += -g ${LIBS}

# compiler and linker
# CC = cc
