CAT := cat
EXPR := expr
GIT := git
GREP := grep
PRINTF := printf
SED := sed

# VERSION FORMAT: v<VERSIONNO>p<PATCHNO> (Example: v0.1p0)
VERSION = $(shell $(CAT) VERSION)
VERSIONNO = $(shell $(CAT) VERSION | $(SED) 's/v\([0-9]*\.[0-9]*\)p.*$$/\1/' )
PATCHNO = $(shell $(CAT) VERSION | $(SED) 's/^.*p//' )

gitpush___stamp: gittag___stamp
	$(GIT) push origin main --tags
	$(GIT) push github main --tags
	$(GIT) rev-parse HEAD > $@

gittag___stamp: version_up___stamp
	$(GIT) tag $(VERSION) main
	$(PRINTF) "%s\n" "$(VERSION)" > $@

version_up___stamp: VERSION
	if [ -n "$$($(GIT) status -s)" ]; then $(PRINTF) "Error: commit, first.\n"; exit 1; fi
	$(GIT) tag --points-at main | $(GREP) -qx "$(VERSION)" || while $(GIT) rev-parse -q --verify $$($(CAT) VERSION) >/dev/null 2>&1; do $(PRINTF) "v%.1fp%02d\n" $(VERSIONNO) $$($(EXPR) $(PATCHNO) + 1) >$<; done
	$(GIT) add VERSION
	$(GIT) commit -m "version up"
	$(CAT) $< >$@

gitclean:
	$(RM) *___stamp
