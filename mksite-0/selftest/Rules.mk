MKSITE ?= perl ../../mksite.pl
U ?= -U0
X ?= -I name=.formatter
diff ?= diff
sort ?= sort
sortuniq ?= sort -u # sort | uniq

sh :
	$(MAKE) check "MKSITE=sh ../../mksite.sh"
pl :
	$(MAKE) check "MKSITE=perl ../../mksite.pl"

TESTED :
	@ for i in *.html */*.html */*/*.html */*/*/*.html */*/*/*/*.html \
	           *.xml  */*.xml  */*/*.xml  */*/*/*.xml  */*/*/*/*.xml  \
	; do if test -f "$$i" \
	; then echo cp "$$i" "$$i.test" ; cp "$$i" "$$i.test" \
	; fi done
CHECK :
	test ! -d DEBUG || rm -rf DEBUG ; mkdir DEBUG
	$(MKSITE) 
	@ BAD="" \
	; for i in `find . -name \*.test | $(sortuniq)` \
	; do echo $(diff) $X $U $$i `basename $$i .test` \
	; if      $(diff) $X $U $$i `basename $$i .test` \
	; then echo "= $@ $$i OK" \
	; else echo "= $@ $$i FAILED" ; BAD="$$BAD|$$i" \
	; fi done \
	; if test ".$$BAD" = "." ; then echo "= $@ = OK" \
	; else FS="|" ; for i in "$$BAD" ; do echo "BAD $$i" ; done \
	; exit 1 ; fi
