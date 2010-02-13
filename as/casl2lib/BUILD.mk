.PHONY: clean

$(TARGET): $(SRCFILE)
	@echo ";;; *** This file is auto generated from $(SRCFILE) of `pwd | xargs basename` ***" >$@; \
     cat $^ >>$@
clean:
	@rm $(TARGET)
