AUTOMAKE_OPTIONS = subdir-objects

TESTS = test_contrac

check_PROGRAMS = $(TESTS)
AM_CFLAGS = -Wall -Werror -I"../include" -pthread @CHECK_CFLAGS@ @LIBCONTRAC_CFLAGS@
LDADD = ../libcontrac.la @CHECK_LIBS@ @LIBCONTRAC_LIBS@


.PHONY: check-valgrind ;
check-valgrind: $(TESTS)
	@for test in $$(echo $(TESTS) | sed 's/tests\//tests\/.libs\//g') ; do \
		CK_FORK=no $(VALGRIND) --error-exitcode=1 --leak-resolution=low --quiet \
		--leak-check=full --show-possibly-lost=no --suppressions=valgrind.suppressions .libs/lt-$${test} ; \
	done

