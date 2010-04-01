AUTOMAKE_OPTIONS = foreign
DEFAULT_INCLUDES = -I$(top_builddir)/inc -I$(top_srcdir)/inc

bin_PROGRAMS = gzlib

gzlib_SOURCES = gzlib.c 
gzlib_LDADD= ../lib/libgz.la

# ---------------------------------------------------
check:
	objdump -T .libs/gzlib
	./gzlib -l $(srcdir)/test.c
	./gzlib -l $(srcdir)/test.c.gz
	ls -l $(srcdir)/test.c $(srcdir)/test.c.gz
	cp $(srcdir)/test.c tmp.c
	./gzlib gzip tmp.c
	cp tmp.c.gz tmp.c1.gz
	./gzlib gunzip tmp.c1.gz
	diff -us tmp.c tmp.c1
	cp tmp.c.gz tmp.c2.gz
	- gunzip -f tmp.c2.gz
	- diff -us tmp.c tmp.c2
	- rm tmp.*

EXTRA_DIST = test.c test.c.gz

# ---------------------------------------------------
.c.i :
	$(COMPILE) -E $< >$@
