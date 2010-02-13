SRCDIR = `ls -d src_*`
TESTDIR = test
.PHONY: all clean
all:
	@for target in $(SRCDIR); do $(MAKE) -C $$target; done
clean:
	@for target in $(SRCDIR); do $(MAKE) -C $$target clean; done;
     make -C $(TESTDIR) clean
check:
	@make -C $(TESTDIR)
