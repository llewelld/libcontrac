EXTRA_DIST = include/contrac

SUBDIRS = src tests
dist_doc_DATA = AUTHORS ChangeLog COPYING INSTALL NEWS README

# Install the pkg-config file; the directory is set using
# PKG_INSTALLDIR in configure.ac.
pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = libcontrac-0.pc

export VERSION

ChangeLog:
	touch $@

.PHONY: doxygen

doxygen:
	$(DOXYGEN) doxyfile
#	@if test "$DOXYGEN" = doxygen; then \
#		$(DOXYGEN) doxyfile; \
#	else \
#		echo "Please install doxygen to generate the documentation."; \
#	fi

