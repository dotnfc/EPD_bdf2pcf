# 
#  Copyright 2005  Red Hat, Inc.
# 
#  Permission to use, copy, modify, distribute, and sell this software
#  and its documentation for any purpose is hereby granted without fee,
#  provided that the above copyright notice appear in all copies and that
#  both that copyright notice and this permission notice appear in
#  supporting documentation, and that the name of Red Hat not be used in
#  advertising or publicity pertaining to distribution of the software
#  without specific, written prior permission.  Red Hat makes no
#  representations about the suitability of this software for any
#  purpose.  It is provided "as is" without express or implied warranty.
# 
#  RED HAT DISCLAIMS ALL WARRANTIES WITH REGARD TO THIS SOFTWARE,
#  INCLUDING ALL IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS, IN
#  NO EVENT SHALL RED HAT BE LIABLE FOR ANY SPECIAL, INDIRECT OR
#  CONSEQUENTIAL DAMAGES OR ANY DAMAGES WHATSOEVER RESULTING FROM LOSS OF
#  USE, DATA OR PROFITS, WHETHER IN AN ACTION OF CONTRACT, NEGLIGENCE OR
#  OTHER TORTIOUS ACTION, ARISING OUT OF OR IN CONNECTION WITH THE USE OR
#  PERFORMANCE OF THIS SOFTWARE.

SUBDIRS = man
bin_PROGRAMS = bdftopcf

AM_CFLAGS = $(BDFTOPCF_CFLAGS) $(CWARNFLAGS)
bdftopcf_LDADD = $(BDFTOPCF_LIBS)

bdftopcf_SOURCES = \
	atom.c \
	bdfint.h \
	bdfread.c \
	bdfutils.c \
	bitmap.c \
	bitmap.h \
	bitmaputil.c \
	bufio.c \
	bufio.h \
	config.h \
	defaults.c \
	fileio.c \
	filewr.c \
	fntfil.h \
	fntfilio.h \
	fntfilst.h \
	font.h \
	fontaccel.c \
	fontink.c \
	fontmisc.h \
	fontutil.h \
	fontxlfd.h \
	pcf.h \
	pcfwrite.c \
	private.c \
	utilbitmap.c \
	bdftopcf.c

MAINTAINERCLEANFILES = ChangeLog INSTALL

.PHONY: ChangeLog INSTALL

INSTALL:
	$(INSTALL_CMD)

ChangeLog:
	$(CHANGELOG_CMD)

dist-hook: ChangeLog INSTALL

if LINT
ALL_LINT_FLAGS=$(LINT_FLAGS) $(DEFS) $(DEFAULT_INCLUDES) $(INCLUDES) \
                $(AM_CPPFLAGS) $(CPPFLAGS) $(bdftopcf_CFLAGS)

lint:
	$(LINT) $(ALL_LINT_FLAGS) $(bdftopcf_SOURCES)
endif LINT

EXTRA_DIST = README.md
