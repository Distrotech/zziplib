MKSITE ?= perl ../../mksite.pl
U = -U0
X = -I name=formatter -I generated.sitemap.index
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

