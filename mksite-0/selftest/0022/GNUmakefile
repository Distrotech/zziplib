MKSITE ?= perl ../../mksite.pl
U = -U0
X = -I name=.formatter -I generated.sitemap.index -I DC.date
diff = diff

PAGES = page1.html
sortuniq = sort -u # sort | uniq
check :
	test ! -d DEBUG || rm -rf DEBUG ; mkdir DEBUG
	$(MKSITE) 
	@ for i in `ls *.html */*.html $(PAGES) | $(sortuniq)` \
	; do echo $(diff) $X $U $$i.test $$i \
	; if      $(diff) $X $U $$i.test $$i \
	; then echo "= $@ $$i OK" \
	; else echo "= $@ $$i FAILED" ; exit 1 \
	; fi done

sh :
	$(MAKE) check "MKSITE=sh ../../mksite.sh"
pl :
	$(MAKE) check "MKSITE=perl ../../mksite.pl"
tested :
	@ for i in *.html */*.html */*/*.html */*/*/*.html */*/*/*/*.html \
	; do if test -f "$$i" \
	; then echo cp "$$i" "$$i.test" ; cp "$$i" "$$i.test" \
	; fi done
