# spt version
VERSION = 0.1

# Customize below to fit your system
#USE_LIBNOTIFY = -DUSE_LIBNOTIFY
#LIBNOTIFY_CFLAGS =  `pkg-config --cflags libnotify`
#LIBNOTIFY_LIBS = `pkg-config --libs libnotify`

# paths
PREFIX = /usr/local
MANPREFIX = ${PREFIX}/share/man
# includes and libs
INCS = -I. -I/usr/include ${USE_LIBNOTIFY} ${LIBNOTIFY_CFLAGS}
LIBS = -L/usr/lib ${LIBNOTIFY_LIBSS}

# flags
CPPFLAGS = -DVERSION=\"${VERSION}\"
CFLAGS += -g -std=c99 -pedantic -Wall -Os ${INCS} ${CPPFLAGS}
LDFLAGS += -g ${LIBS}

# compiler and linker
# CC = cc
