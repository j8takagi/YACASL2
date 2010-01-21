prefix = $(HOME)
.PHPNY: all clean check install uninstall
all:
	make -C src
check:
	make -C test/integration
clean:
	make -C src clean
	make -C test/integration clean
	make -C test/unit clean
install: all
	@if test -d $(prefix); then \
       install -s ./casl2 $(prefix)/casl2; \
       install -s ./comet2 $(prefix)/comet2; \
       install -s ./dumpword $(prefix)/dumpword; \
     fi
uninstall:
	@if test -d $(prefix); then \
       rm -f $(prefix)/casl2 $(prefix)/comet2 $(prefix)/dumpword; \
     fi
