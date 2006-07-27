MKSITE ?= perl ../../mksite.pl
U ?= -U0
X ?= -I name=.formatter -I DC.date.modified
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
CLEAN :
	test ! -d DEBUG || rm -rf DEBUG ; mkdir DEBUG
	@ for i in *.html */*.html */*/*.html */*/*/*.html */*/*/*/*.html \
	           *.xml  */*.xml  */*/*.xml  */*/*/*.xml  */*/*/*/*.xml \
	; do if test -f "$$i" ; then echo "rm $$i" ; rm "$$i" ; fi done
CHECK :
	$(MAKE) clean || $(MAKE) CLEAN
	$(MAKE) prepare || true
	$(MKSITE) $(MKSITEFLAGS)
	$(MAKE) test || $(MAKE) TEST
TEST :
	@ BAD="" ; OK="" \
	; for i in `find . -name \*.test | $(sortuniq)` \
	; do echo $(diff) $X $U $$i `echo "$$i" | sed -e "s|.test$$||"` \
	; if      $(diff) $X $U $$i `echo "$$i" | sed -e "s|.test$$||"` \
	; then OK="$$OK|$$i" \
	; else BAD="$$BAD|$$i" ; echo "= $@ $$i FAILED" \
	; fi done \
	; if test ".$$BAD" = "." \
	; then files=`echo "$$OK" | sed -e "s:[^|]*::g" | wc -c` \
	;  echo "= $@ $$files files OK" \
	; else IFS="|" \
	; for i in $$OK  ; do echo " OK $$i" ; done \
	; for i in $$BAD ; do echo "BAD $$i" ; done \
	; exit 1 ; fi
