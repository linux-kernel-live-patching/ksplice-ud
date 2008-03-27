SUBDIRS = libudis86 . udcli . tests

.PHONY: libudis86 udcli tests

libudis86:
	$(MAKE) -C $@

udcli: libudis86
	$(MAKE) -C $@

tests: libudis86 udcli
	$(MAKE) -C $@ tests

maintainer-clean-local:
	$(RM) -rf autom4te.cache aclocal.m4 
