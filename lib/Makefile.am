AUTOMAKE_OPTIONS = foreign
DEFAULT_INCLUDES = -I$(top_builddir)/inc -I$(top_srcdir)/inc

lib_LTLIBRARIES = libgz.la

VERSION_INFO=@VERSION_INFO@
RELEASE_INFO=@RELEASE_INFO@

libgz_la_SOURCES = gzio.c 
libgz_la_LDFLAGS= $(RELEASE_INFO) $(VERSION_INFO)
libgz_la_LIBADD= -lz

# ---------------------------------------------------
check:
	objdump -T .libs/libgz.so

# ---------------------------------------------------
.c.i :
	$(COMPILE) -E $< >$@
