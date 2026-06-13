CAT := cat
EXPR := expr
GIT := git
PRINTF := printf
SED := sed

# VERSION FORMAT: v<VERSIONNO>p<PATCHNO> (Example: v0.1p00)
VERSION = $(shell $(CAT) VERSION)
VERSIONNO = $(shell $(CAT) VERSION | $(SED) 's/v\([0-9]*\.[0-9]*\)p.*$$/\1/' )
PATCHNO = $(shell $(CAT) VERSION | $(SED) 's/^.*p//' )

gitpush___stamp: gittag___stamp
	$(GIT) push origin main --follow-tags
	$(GIT) push github main --follow-tags
	$(GIT) rev-parse HEAD > $@

gittag___stamp: commit___stamp
	$(GIT) tag $(VERSION) main
	$(PRINTF) "%s\n" "$(VERSION)" > $@

commit___stamp: version_up___stamp
	$(GIT) add VERSION
	$(GIT) commit
	$(GIT) rev-parse HEAD > $@

version_up___stamp: VERSION
	if [ -n "$$($(GIT) status -s)" ]; then $(PRINTF) "Error: commit, first.\n"; exit 1; fi
	while $(GIT) rev-parse -q --verify $$($(CAT) VERSION) >/dev/null 2>&1; do $(PRINTF) "v%sp%s\n" $(VERSIONNO) $$($(EXPR) $(PATCHNO) + 1) >$<; done
	$(CAT) $< >$@

gitclean:
	$(RM) *___stamp
