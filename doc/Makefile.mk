MAINTAINERCLEANFILES += doc/esskyuehl.dox

PACKAGE_DOCNAME = $(PACKAGE_TARNAME)-$(PACKAGE_VERSION)-doc

if EFL_BUILD_DOC

doc-clean:
	rm -rf doc/html/ doc/latex/ doc/man/ doc/xml/ doc/$(PACKAGE_DOCNAME).tar*

doc: all doc-clean
	$(efl_doxygen)
	cp doc/img/* doc/html/
	rm -rf doc/$(PACKAGE_DOCNAME).tar*
	mkdir -p doc/$(PACKAGE_DOCNAME)/doc
	cp -R doc/html/ doc/latex/ doc/man/ doc/$(PACKAGE_DOCNAME)/doc
	tar cf doc/$(PACKAGE_DOCNAME).tar doc/$(PACKAGE_DOCNAME)/
	bzip2 -9 doc/$(PACKAGE_DOCNAME).tar
	rm -rf doc/$(PACKAGE_DOCNAME)/
	mv doc/$(PACKAGE_DOCNAME).tar.bz2 $(top_srcdir)

clean-local: doc-clean

else

doc:
	@echo "Documentation not built. Run ./configure --help"

endif

EXTRA_DIST += \
doc/Doxyfile.in \
doc/img/edoxy.css \
doc/img/header_menu_current_background.png \
doc/img/foot_bg.png \
doc/img/header_menu_unselected_background.png \
doc/img/header_menu_background_last.png \
doc/img/head_bg.png \
doc/img/logo.png \
doc/img/header_menu_background.png \
doc/e.css \
doc/head.html \
doc/foot.html \
doc/esskyuehl.dox.in
