/** \ingroup contrac
 * @file
 * @author	David Llewellyn-Jones
 * @version	$(VERSION)
 *
 * @section LICENSE
 *
 *
 *
 * @brief
 * @section DESCRIPTION
 *
 *
 *
 */

// Includes

#include <check.h>

#include "contrac/contrac.h"

// Defines

// Structures

// Function prototypes

// Function definitions

START_TEST (check_contrac) {

}
END_TEST

int main(void) {
	int failed;
	Suite * s;
	SRunner *sr;
	TCase * tc;

	s = suite_create("libcontrac");

	tc = tcase_create("Contrac");
	tcase_add_test(tc, check_contrac);
	suite_add_tcase(s, tc);
	sr = srunner_create(s);

	srunner_run_all(sr, CK_NORMAL);
	failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	
	return (failed == 0) ? 0 : -1;
}

