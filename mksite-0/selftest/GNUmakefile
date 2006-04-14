check :
	@ for i in 0* \
	; do echo $(MAKE) -C $$i $@ \
	; if      $(MAKE) -C $$i $@ \
	; then echo = $@ $$i OK \
	; else echo = $@ $$i FAILED ; exit 1 \
	; fi done

sh :
	$(MAKE) check "MKSITE=sh ../../mksite.sh"
pl :
	$(MAKE) check "MKSITE=perl ../../mksite.pl"
