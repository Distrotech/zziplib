X = -I name=.formatter -I generated.sitemap.index -I DC.date

include ../Rules.mk

check : CHECK
tested : TESTED
prepare :
	(cd test && $(subst ../../,../../../,$(MKSITE)) subsite.htm)
