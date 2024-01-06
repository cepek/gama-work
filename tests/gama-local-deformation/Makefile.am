EXTRA_DIST = CMakeLists.txt \
             gama-local-deformation-diff.sh_ \
             input/diff/data-1-2.svg \
             input/diff/data-1-2.txt \
             input/diff/data-1.gkf \
             input/diff/data-1.xml \
             input/diff/data-2.gkf \
             input/diff/data-2.xml

TESTS = gama-local-deformation-diff.sh

GAMA_SRC     = $(srcdir)/src
GAMA_INPUT   = $(srcdir)/input
GAMA_RESULTS = $(srcdir)/results/$(PACKAGE_VERSION)
GAMA_OTHERS  = Makefile $(GAMA_RESULTS)

do_subst = sed \
-e 's,[@]abs_top_builddir[@],$(abs_top_builddir),g' \
-e 's,[@]abs_top_srcdir[@],$(abs_top_srcdir),g' \
-e 's,[@]PACKAGE_VERSION[@],$(PACKAGE_VERSION),g' \
-e 's,[@]INPUT_FILES[@],$(INPUT_FILES),g' \
-e 's,[@]GAMA_INPUT[@],$(GAMA_INPUT),g' \
-e 's,[@]GAMA_RESULTS[@],$(GAMA_RESULTS),g' \
-e 's,[@]GAMA_XML[@],$(top_srcdir)/xml,g' \
-e 's,[@]BUG_FILES[@],$(BUG_FILES),g' \
-e 's,[@]CTU_FILES[@],$(CTU_FILES),g' \
-e 's,[@]KRUMM_FILES[@],$(KRUMM_FILES),g' \
-e 's,[@]FROMTODH_FILES[@],$(FROMTODH_FILES),g' \
-e 's,[@]YAML_FILES[@],$(YAML_FILES),g' \
-e 's,[@]GNU_GAMA_LOCAL_TEST_XMLLINT[@],$(GNU_GAMA_LOCAL_TEST_XMLLINT),g'

clean-local: clean-local-check
.PHONY: clean-local-check
clean-local-check:
	-rm -rf $(GAMA_RESULTS) $(TESTS) results

$(GAMA_RESULTS):
	$(MKDIR_P) $(GAMA_RESULTS)/diff

gama-local-deformation-diff.sh: $(srcdir)/gama-local-deformation-diff.sh_
	@$(do_subst) < $(srcdir)/gama-local-deformation-diff.sh_ \
	             > gama-local-deformation-diff.sh
	@chmod +x gama-local-deformation-diff.sh
