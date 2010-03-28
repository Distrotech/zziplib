AUTOMAKE_OPTIONS = foreign dist-bzip2
ACLOCAL_AMFLAGS = -I m4
SUBDIRS =      inc lib bin

EXTRA_DIST = $(PACKAGE).spec
rpm: dist-bzip2 $(PACKAGE).spec
	rpmbuild -ta $(PACKAGE)-$(VERSION).tar.bz2
	test -d ../pub && cp $(PACKAGE)-$(VERSION).tar.bz2 ../pub/
