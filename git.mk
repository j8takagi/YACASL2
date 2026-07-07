CAT := cat
EXPR := expr
GIT := git
GREP := grep
PRINTF := printf
SED := sed


gitpush___stamp: gittag___stamp
	$(GIT) push origin main --tags
	$(GIT) push github main --tags
	$(GIT) rev-parse HEAD > $@

gittag___stamp: patchup___stamp
	$(GIT) tag --points-at HEAD | $(GREP) -qx "$(VERSION)" || $(GIT) tag $(VERSION) main
	$(PRINTF) "%s\n" "$(VERSION)" > $@

patchup___stamp:
	git diff-index --quiet HEAD -- || ($(PRINTF) "Error: commit, first.\n"; exit 1)
# VERSION FORMAT: v<VERSIONNO>p<PATCHNO> (Example: v0.1p0)
	$(GIT) tag --points-at HEAD | $(GREP) -qx "$$(cat VERSION)" && exit 0 || while $(GIT) rev-parse -q --verify $$(cat VERSION) >/dev/null 2>&1; do $(PRINTF) "v%.1fp%02d\n" $$($(SED) 's/v\([0-9]*\.[0-9]*\)p.*$$/\1/' VERSION) $$($(EXPR) $$($(SED) 's/^.*p//' VERSION)  + 1) >VERSION; done;
	git diff-index --quiet HEAD -- || $(GIT) commit -a -m "version up to $(VERSION)"
	$(PRINTF) "%s\n" "$(VERSION)" >$@

gitclean:
	$(RM) *___stamp
