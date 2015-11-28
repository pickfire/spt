# spt version
VERSION = 0.0

# Customize below to fit your system

# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man

# includes and libs
INCS = -I. -I/usr/include \
       `pkg-config --cflags libnotify`
LIBS = -L/usr/lib \
       `pkg-config --libs libnotify`

# flags
CPPFLAGS = -DVERSION=\"${VERSION}\"
CFLAGS += -g -std=c99 -pedantic -Wall -O0 ${INCS} ${CPPFLAGS}
LDFLAGS += -g ${LIBS}

# compiler and linker
# CC = cc
