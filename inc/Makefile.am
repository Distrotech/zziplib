AUTOMAKE_OPTIONS = foreign

pkginclude_HEADERS = gzstdio.h gzio.h gzconfig.h
BUILT_SOURCES = gzconfig.h

pkgconfigdir=$(libdir)/pkgconfig
pkgconfig_HEADERS= libgz.pc

# ----------------------------------------------------------------------
libgz.pc : Makefile
	@ echo "prefix=$(prefix)" >$@
	@ echo "libdir=$(libdir)" >>$@
	@ echo "pkgincludedir=$(pkgincludedir)" >>$@
	@ echo " " >>$@
	@ echo "Name: $(PACKAGE)" >>$@
	@ echo "Version: $(VERSION)" >>$@
	@ head $(top_srcdir)/$(PACKAGE).spec \
	| grep Summary | sed -e s,Summary,Description, >> $@
	echo 'Libs: -L$${libdir} -lgz -lz' >>$@
	echo 'Cflags: -I$${pkgincludedir}' >>$@
