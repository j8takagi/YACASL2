SRCDIR = `ls -d *src`
.PHONY: all clean
all:
	@for target in $(SRCDIR); do $(MAKE) -C $$target; done
clean:
	@for target in $(SRCDIR); do $(MAKE) -C $$target clean; done
