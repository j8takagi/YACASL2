ECHOHEADER = 'echo ";;; *** This file is auto generated. ***"'
TARGETS = inl.casl l2str.casl mula.casl outa.casl outb.casl outl.casl
.PHONY: all clean
all: $(TARGETS)
inl.casl: src_inl/inl_main.casl str2l.casl
	@echo ";;; *** This file is auto generated. ***" >$@; \
     cat $^ >>$@
l2str.casl: src_l2str/l2str_main.casl divl.casl rev.casl
	@echo ";;; *** This file is auto generated. ***" >$@; \
     cat $^ >>$@
mula.casl: src_mula/mula_main.casl mull.casl
	@echo ";;; *** This file is auto generated. ***" >$@; \
     cat $^ >>$@
outa.casl: src_outa/outa_main.casl divl.casl rev.casl abs.casl
	@echo ";;; *** This file is auto generated. ***" >$@; \
     cat $^ >>$@
outb.casl: src_outb/outb_main.casl divl.casl rev.casl
	@echo ";;; *** This file is auto generated. ***" >$@; \
     cat $^ >>$@
outl.casl: src_outl/outl_main.casl l2str.casl
	@echo ";;; *** This file is auto generated. ***" >$@; \
     cat $^ >>$@
str2l.casl: src_str2l/str2l_main.casl mull.casl
	@echo ";;; *** This file is auto generated. ***" >$@; \
     cat $^ >>$@
clean:
	@for target in $(TARGETS); do rm -f $$target; done
