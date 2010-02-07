.PHONY: clean

$(TARGET): $(SRCFILE)
	@cat $^ >$@
clean:
	@rm $(TARGET)
