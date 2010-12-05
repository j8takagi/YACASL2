ifndef prefix
    prefix = $(HOME)
endif
CLEANDIR = src test/integration test/unit as/casl2lib
.PHONY: all build clean check doc install uninstall

all: build

build:
	make -C src

check:
	@make -sC test/integration

doc:
	@make -sC doc

clean:
	@for target in $(CLEANDIR); do $(MAKE) -sC $$target clean; done; \
     rm -f casl2 comet2 dumpword

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
