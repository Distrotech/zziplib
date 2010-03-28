AUTOMAKE_OPTIONS = foreign
DEFAULT_INCLUDES = -I$(top_builddir)/inc -I$(top_srcdir)/inc

bin_PROGRAMS = gzlib

gzlib_SOURCES = gzlib.c 
gzlib_LDADD= ../lib/libgz.la

# ---------------------------------------------------
check:
	objdump -T .libs/gzlib
	./gzlib -l test.c
	./gzlib -l test.c.gz
	ls -l test.c test.c.gz

EXTRA_DIST = test.c test.c.gz

# ---------------------------------------------------
.c.i :
	$(COMPILE) -E $< >$@
