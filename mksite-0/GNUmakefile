all : index.html

SUBDIRS =
PACKAGE = mksite

TODAYSDATE := $(shell date +%Y.%m%d)
TODAYSHOUR := $(shell date +%Y.%m%d.%H%M)
VERSION=$(TODAYSDATE)
SNAPSHOT=$(TODAYSHOUR)
DISTNAME = $(PACKAGE)-$(VERSION)
SNAPSHOTNAME = $(PACKAGE)-$(SNAPSHOT)
SAVETO = _dist

DISTFILES = GNUmakefile README.TXT COPYING.ZLIB $(PACKAGE).spec \
            mksite.txt mksite.sh mksite.pl \
            doc/*.htm doc/*.gif test*/*.htm 

diffs = diff -U1
known = -e "/formatter/d"

test : test1x test2x
test1x : test1x.html
	for i in $@/*.html ; do orig=`echo $$i | sed -e "s|x/|/|"` \
	; sed $(known) $$orig >$$i.orig     ; sed $(known) $$i     >$$i.made \
	; $(diffs) $$i.orig $$i.made ; done ; rm $@/*.orig test1x/*.made
test2x : test2x.html
	for i in $@/*.html ; do orig=`echo $$i | sed -e "s|x/|/|"` \
	; sed $(known) $$orig >$$i.orig     ; sed $(known) $$i     >$$i.made \
	; $(diffs) $$i.orig $$i.made ; done ; rm $@/*.orig test1x/*.made

HTMLPAGES= [_A-Za-z0-9-][/_A-Za-z0-9-]*[.]html

test1.html : test1/*.htm mksite.sh
	cd test1 && sh ../mksite.sh site.htm
	sed -e "s|href=\"\\($(HTMLPAGES)\"\\)|href=\"test1/\\1|" \
	    test1/index.html > $@
	sleep 3 # done $@
test2.html : test2/*.htm mksite.sh
	cd test2 && sh ../mksite.sh site.htm
	sed -e "s|href=\"\\($(HTMLPAGES)\"\\)|href=\"test2/\\1|" \
	    test2/index.html > $@
	sleep 3 # done $@

test1x.html : test1/*.htm mksite.pl GNUmakefile
	test ! -d test1x/ || rm -r test1x/
	mkdir test1x && cp -a test1/*.htm test1x/
	cd test1x && perl ../mksite.pl site.htm
	sed -e "s|href=\"\\($(HTMLPAGES)\"\\)|href=\"test1x/\\1|" \
	    test1x/index.html > $@
	sleep 2 # done $@
test2x.html : test2/*.htm mksite.pl GNUmakefile
	test ! -d test2x/ || rm -r test2x/
	mkdir test2x && cp -a test2/*.htm test2x/
	- rm test2x/*.print.* ; mkdir test2x/DEBUG
	cd test2x && perl ../mksite.pl site.htm
	sed -e "s|href=\"\\($(HTMLPAGES)\"\\)|href=\"test2x/\\1|" \
	    test2x/index.html > $@
	sleep 2 # done $@

test1 test2  site : .FORCE ; rm $@.html ; $(MAKE) $@.html
doc.html : doc/*.htm mksite.sh
	cd doc && sh ../mksite.sh site.htm        "-VERSION=$(VERSION)"
	cd doc && sh ../mksite.sh features.htm    "-VERSION=$(VERSION)"
	sed -e "s|href=\"\\($(HTMLPAGES)\"\\)|href=\"doc/\\1|" \
	    doc/index.html > $@
	sleep 5 # done $@

print.html : doc/*.htm mksite.sh
	cd doc && sh ../mksite.sh site.htm -print "-VERSION=$(VERSION)"
	sed -e "s|href=\"\\($(HTMLPAGES)\"\\)|href=\"doc/\\1|" \
	    doc/index.print.html > $@

check : test1.html test2.html test1x.html test2x.html
index.html : doc.html check
	cp doc.html index.html

site.html : *.htm mksite.sh
	sh mksite.sh site.htm

ff :
	cd doc && sh ../mksite.sh features.htm

clean : 
	- rm *.html */*.html */*.tmp

distdir = $(PACKAGE)-$(VERSION)
distdir :
	test -d $(distdir) || mkdir $(distdir)
	- $(MAKE) distdir-hook
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
SUMMARY = The mksite.sh documentation formatter
RPM_MAKE_ARGS = prefix=%_prefix DESTDIR=%buildroot VERSION=%version
distdir-hook : $(PACKAGE).spec
$(PACKAGE).spec : $(distdir)
	echo "Name: $(PACKAGE)" > $@
	echo "Summary: $(SUMMARY)" >> $@
	echo "Version: $(VERSION)" >> $@
	echo "Release: 1" >> $@
	echo "License: ZLIB" >> $@
	echo "Group: Productivity/Networking/Web" >> $@
	echo "URL: http://zziplib.sf.net/mksite" >> $@
	echo "BuildRoot:  /tmp/%{name}-%{version}" >> $@
	echo "Source: $(DISTNAME).tar.bz2" >> $@
	echo " " >> $@
	echo "%package sh" >> $@
	echo "Summary: $(SUMMARY) script" >> $@
	echo "Group: Productivity/Networking/Web" >> $@
	echo "Provides: mksitesh " >> $@
	echo "%package doc" >> $@
	echo "Summary: $(SUMMARY) webpages" >> $@
	echo "Group: Productivity/Networking/Web" >> $@
	echo "BuildRequires: perl" >> $@
	echo " " >> $@
	echo "%description" >> $@
	head -6 mksite.sh | sed -e "s/^../    /" >> $@
	echo "%description sh" >> $@
	head -6 mksite.sh | sed -e "s/^../    /" >> $@
	echo "%description doc" >> $@
	head -6 mksite.sh | sed -e "s/^../    /" >> $@
	echo " " >> $@
	echo "%prep" >> $@
	echo "%setup -q" >> $@
	echo "%build" >> $@
	echo "make $(RPM_MAKE_ARGS)" >> $@
	echo "%install" >> $@
	echo "make install-data $(RPM_MAKE_ARGS)" >> $@
	echo "make install-programs $(RPM_MAKE_ARGS)" >> $@
	echo "%clean" >> $@
	echo "rm -rf %buildroot" >> $@
	echo "%files sh" >> $@
	echo "%_bindir/*" >> $@
	echo "%files doc" >> $@
	echo "%_datadir/*" >> $@

rpm : dist
	rpmbuild -ta --target noarch $(SAVETO)/$(DISTNAME).tar.bz2

# -----------------------------------------------------------------------
INSTALLDIRS =
INSTALLFILES = doc/*.html doc/site.htm doc/site.print.htm doc/mksite-icon.gif \
               mksite.sh mksite.txt COPYING.ZLIB 
INSTALLTARBALL = _dist/$(DISTNAME).tar.bz2
INSTALLPROGRAMS = mksite.sh mksite.pl

prefix  ?= /usr
datadir ?= $(prefix)/share
bindir  ?= $(prefix)/bin
mkinstalldirs ?= mkdir -p
INSTALL_DATA ?= cp -p
INSTALL_SCRIPT ?= cp -p
htmpath = groups/z/zz/zziplib/htdocs/mksite
docdir = $(datadir)/$(htmpath)
install-data : doc.html # $(htm_FILES:.htm=.html)
	$(mkinstalldirs) $(DESTDIR)$(docdir)
	@ for i in $(INSTALLDIRS) ; do test -d $$i || continue \
	; echo cp -pR "$$i" $(DESTDIR)$(docdir) \
	; cp -pR $$i $(DESTDIR)$(docdir) \
	; chmod -R a+r $(DESTDIR)$(docdir)/$$i \
	; done
	find $(DESTDIR)$(docdir) -name "*~" -exec rm {} \;
	find $(DESTDIR)$(docdir) -name "*.tmp" -exec rm {} \;
	$(INSTALL_DATA) $(INSTALLFILES) $(DESTDIR)$(docdir)
install-tarball :
	$(mkinstalldirs) $(DESTDIR)$(docdir)
	$(INSTALL_DATA) $(INSTALLTARBALL) $(DESTDIR)$(docdir)
install-programs : 
	$(mkinstalldirs) $(DESTDIR)$(bindir)
	$(INSTALL_SCRIPT) $(INSTALLPROGRAMS) $(DESTDIR)$(bindir)
install : install-data install-tarball
	echo "# skipped  make install-programs"

WWWNAME= guidod
WWWHOST= shell.sourceforge.net
WWWPATH= /home/$(htmpath)
TMPPATH=/tmp/$(PACKAGE)-$(VERSION)
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

mksite_.pl : mksite.sh GNUmakefile mksiteperl.pl
	perl mksiteperl.pl $< > $@

q : mksite_.pl
	diff -U0 mksite.pl mksite_.pl

guidod-hub:
	cp mksite.sh mksite.pl ../guidod-hub
	$(MAKE) -C ../guidod-hub
	$(MAKE) -C ../guidod-hub check

