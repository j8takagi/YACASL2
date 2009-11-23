INSTALLDIR = /usr/local/bin
.PHPNY: all clean check install uninstall
all:
	make -C src ../casl2 ../comet2
check:
	make -C test/astest
clean:
	make -C src clean
	make -C test/astest clean
	make -C test/utest clean
install: casl2 comet2
	@if test -d $(INSTALLDIR); then \
       install -s ./casl2 $(INSTALLDIR)/casl2; \
       install -s ./comet2 $(INSTALLDIR)/comet2; \
     fi
uninstall:
	@if test -d $(INSTALLDIR); then \
       rm -f $(INSTALLDIR)/casl2 $(INSTALLDIR)/comet2; \
     fi
