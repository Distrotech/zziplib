all : index.html

HTMLPAGES= [_A-Za-z0-9-][/_A-Za-z0-9-]*[.]html

test1.html : test1/*.htm mksite.sh
	cd test1 && sh ../mksite.sh site.htm
	sed -e "s|href=\"\\($(HTMLPAGES)\"\\)|href=\"test1/\\1|" \
	    test2/index.html > $@
test2.html : test2/*.htm mksite.sh
	cd test2 && sh ../mksite.sh site.htm
	sed -e "s|href=\"\\($(HTMLPAGES)\"\\)|href=\"test2/\\1|" \
	    test2/index.html > $@

test1 test2 site : .FORCE ; rm $@.html ; $(MAKE) $@.html
doc.html : doc/*.htm mksite.sh
	cd doc && sh ../mksite.sh site.htm
	sed -e "s|href=\"\\($(HTMLPAGES)\"\\)|href=\"doc/\\1|" \
	    doc/index.html > $@

index.html : test1.html test2.html doc.html
	cp doc.html index.html

site.html : *.htm mksite.sh
	sh mksite.sh site.htm

clean : 
	- rm *.html */*.html */*.tmp

DISTFILES = GNUmakefile README COPYING.ZLIB \
            mksite.txt mksite.sh mksite.gif \
            doc/*.htm test*/*.htm
SUBDIRS =
PACKAGE = mksite

TODAYSDATE := $(shell date +%Y-%m%d)
TODAYSHOUR := $(shell date +%Y-%m%d-%H%M)
DISTNAME = $(PACKAGE)-$(TODAYSDATE)
SNAPSHOTNAME = $(PACKAGE)-$(TODAYSHOUR)
SAVETO = _dist

distdir = $(PACKAGE)-$(TODAYSDATE)

distdir :
	test -d $(distdir) || mkdir $(distdir)
	@ list='$(DISTFILES)' ; for dat in $$list $(EXTRA_DIST) \
	; do dir=`echo "$(distdir)/$$dat" | sed 's|/[^/]*$$||' ` \
	; test -d $$dir || mkdir $$dir ; echo cp -p $$dat $(distdir)/$$dat \
	; cp -p $$dat $(distdir)/$$dat || exit 1 ; done
	@ list='$(SUBDIRS)' ; for dir in $$list $(DIST_SUBDIRS) \
	; do $(MAKE) -C "$$dir" distdir "distdir=../$(distdir)/$$dir" \
	|| exit 1 ; done
	- chmod -R a+r $(distdir)
dist : distdir
	tar cvf $(DISTNAME).tar $(distdir)
	rm -r $(distdir)
	- bzip2 -9 $(DISTNAME).tar
	test -d $(SAVETO) || mkdir $(SAVETO)
	mv $(DISTNAME).tar* $(SAVETO)
snapshot : distdir
	find $(distdir) -name \*.gif -exec rm {} \;
	tar cvf $(SNAPSHOTNAME).tar $(distdir)
	rm -r $(distdir)
	- bzip2 -9 $(SNAPSHOTNAME).tar
	test -d $(SAVETO) || mkdir $(SAVETO)
	mv $(SNAPSHOTNAME).tar* $(SAVETO)

.FORCE :

# -----------------------------------------------------------------------
INSTALLDIRS =
INSTALLFILES = doc/*.html doc/site.htm mksite.sh mksite.txt \
               COPYING.ZLIB _dist/$(DISTNAME).tar.bz2

datadir ?= /usr
mkinstalldirs ?= mkdir -p
INSTALL_DATA ?= cp -p
htmpath = groups/z/zz/zziplib/htdocs/mksite
docdir = $(datadir)/$(webpath)
install : doc.html # $(htm_FILES:.htm=.html)
	$(mkinstalldirs) $(DESTDIR)$(docdir)
	@ for i in $(INSTALLDIRS) ; do test -d $$i || continue \
	; echo cp -pR "$$i" $(DESTDIR)$(docdir) \
	; cp -pR $$i $(DESTDIR)$(docdir) \
	; chmod -R a+r $(DESTDIR)$(docdir)/$$i \
	; done
	find $(DESTDIR)$(docdir) -name "*~" -exec rm {} \;
	find $(DESTDIR)$(docdir) -name "*.tmp" -exec rm {} \;
	$(INSTALL_DATA) $(INSTALLFILES) $(DESTDIR)$(docdir)

WWWNAME= guidod
WWWHOST= shell.sourceforge.net
WWWPATH= /home/$(htmpath)
TMPPATH=/tmp/$(PACKAGE)-$(VERSION)
VERSION=$(TODAYSDATE)
preload :
	$(MAKE) install docdir=$(TMPPATH)
upload : preload
	$(MAKE) install docdir=$(TMPPATH)
	- ssh $(WWWNAME)@$(WWWHOST) mkdir $(WWWPATH)/
	- scp -r $(TMPPATH)/* $(WWWNAME)@$(WWWHOST):$(WWWPATH)/
	rm -r $(TMPPATH)

uploads :
	test -s "$(file)"
	- scp "$(file)" $(WWWNAME)@$(WWWHOST):$(WWWPATH)/$(file)
