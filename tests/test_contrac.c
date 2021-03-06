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
#include <malloc.h>

#include "contrac/contrac.h"
#include "contrac/contrac_private.h"
#include "contrac/utils.h"
#include "contrac/dtk.h"
#include "contrac/rpi.h"
#include "contrac/dtk_list.h"
#include "contrac/rpi_list.h"
#include "contrac/match.h"

// Defines

// Structures

// Function prototypes

// Override time function
static time_t fake_time = 0;
time_t time(time_t *__timer) {
	return fake_time;
}

// Function definitions

START_TEST (check_base64) {
	//bool result;
	//int pos;
	char *string[4] = {
		"This is a string",    // 16
		"Maybe upon a time",   // 17
		"And then there was",  // 18
		"In the end there is", // 19
	};
	char *base64[4] = {
		"VGhpcyBpcyBhIHN0cmluZw==", // 24
		"TWF5YmUgdXBvbiBhIHRpbWU=", // 24
		"QW5kIHRoZW4gdGhlcmUgd2Fz", // 24
		"SW4gdGhlIGVuZCB0aGVyZSBpcw==", // 24
	};
	size_t size;
	char * output;
	int pos;

	for (pos = 0; pos < 4; ++pos) {
		// Encode
		size = base64_encode_size(strlen(string[pos]));
		
		ck_assert(size == (strlen(base64[pos]) + 1));
		output = calloc(sizeof(char), size);

		base64_encode_binary_to_base64((unsigned char *)string[pos], strlen(string[pos]), (unsigned char *)output, &size);

		ck_assert(size == (strlen(base64[pos]) + 1));
		ck_assert_str_eq(output, base64[pos]);

		// Decode
		size = base64_decode_size(strlen(base64[pos]));

		ck_assert(size >= (strlen(string[pos]) + 1));
		ck_assert(size < (strlen(string[pos]) + 4));
		output = calloc(sizeof(char), size + 1);

		base64_decode_base64_to_binary((unsigned char *)base64[pos], strlen(base64[pos]), (unsigned char *)output, &size);

		ck_assert(size >= (strlen(string[pos]) + 1));
		ck_assert(size < (strlen(string[pos]) + 4));
		ck_assert_str_eq(output, string[pos]);
	}
}
END_TEST

START_TEST (check_contrac) {
	bool result;
	unsigned char const *tk;
	int pos;

	// Generate some keys, check the results
	Contrac * contrac;

	contrac = contrac_new();
	ck_assert(contrac != NULL);

	result = contrac_get_initialised(contrac);
	ck_assert(result == false);

	tk = contrac_get_tracing_key(contrac);
	// The tracing key will initialise to zero by default
	for (pos = 0; pos < TK_SIZE; ++pos) {
		ck_assert(tk[pos] == 0);
	}

	result = contrac_generate_tracing_key(contrac);
	ck_assert(result == true);

	result = contrac_get_initialised(contrac);
	ck_assert(result == false);

	// The random generator could generate all zeros, but we'll take the risk
	tk = contrac_get_tracing_key(contrac);
	result = false;
	for (pos = 0; pos < TK_SIZE; ++pos) {
		if (tk[pos] != 0) {
			result = true;
		}
	}
	ck_assert(result == true);

	result = contrac_get_initialised(contrac);
	ck_assert(result == false);

	result = contrac_set_day_number(contrac, 23);
	ck_assert(result == true);

	result = contrac_get_initialised(contrac);
	ck_assert(result == false);

	result = contrac_set_time_interval_number(contrac, 76);
	ck_assert(result == true);

	result = contrac_get_initialised(contrac);
	ck_assert(result == true);

	// Clean up
	contrac_delete(contrac);
}
END_TEST

START_TEST (check_dtk) {
	bool result;
	char const *tracing_key_base64 = "3UmKrtcQ2tfLE8UPSXHb4PtgRfE0E2xdSs+PGVIS8cc=";
	char tk_base64[TK_SIZE_BASE64 + 1];
	char dtk_base64[DTK_SIZE_BASE64 + 1];

	// Generate some keys, check the results
	Contrac * contrac;

	contrac = contrac_new();
	ck_assert(contrac != NULL);

	contrac_set_tracing_key_base64(contrac, tracing_key_base64);

	contrac_get_tracing_key_base64(contrac, tk_base64);
	ck_assert_int_eq(strlen(tk_base64), TK_SIZE_BASE64);
	ck_assert_str_eq(tracing_key_base64, tk_base64);

	// Check the Daily Tracing Keys are generated correctly
	// Should use the standard test vectors when they're available

	result = contrac_set_day_number(contrac, 12);
	ck_assert(result);
	contrac_get_daily_key_base64(contrac, dtk_base64);
	ck_assert_int_eq(strlen(dtk_base64), DTK_SIZE_BASE64);
	ck_assert_str_eq(dtk_base64, "AzZ389DsGecAjZqby1sLNQ==");


	result = contrac_set_day_number(contrac, 0);
	ck_assert(result);
	contrac_get_daily_key_base64(contrac, dtk_base64);
	ck_assert_int_eq(strlen(dtk_base64), DTK_SIZE_BASE64);
	ck_assert_str_eq(dtk_base64, "p7LrsTReTw3k721eIWDjRw==");

	result = contrac_set_day_number(contrac, 143);
	ck_assert(result);
	contrac_get_daily_key_base64(contrac, dtk_base64);
	ck_assert_int_eq(strlen(dtk_base64), DTK_SIZE_BASE64);
	ck_assert_str_eq(dtk_base64, "f6RZL/2wGCzxSBzZc9xVNQ==");

	// Clean up
	contrac_delete(contrac);
}
END_TEST

START_TEST (check_rpi) {
	bool result;
	char const *tracing_key_base64[2] = {
		"3UmKrtcQ2tfLE8UPSXHb4PtgRfE0E2xdSs+PGVIS8cc=",
		"U3CgpSjF0qFW8DNSTHVWF99few5FOW7RV7kA9j6LFTc=",
		};
	char rpi_base64[RPI_SIZE_BASE64 + 1];

	// Generate some keys, check the results
	Contrac * contrac;

	contrac = contrac_new();
	ck_assert(contrac != NULL);

	contrac_set_tracing_key_base64(contrac, tracing_key_base64[0]);

	result = contrac_set_day_number(contrac, 9);
	ck_assert(result);

	// Check the Rolling Proximity Identifiers are generated correctly
	// Should use the standard test vectors when they're available

	result = contrac_set_time_interval_number(contrac, 0);
	ck_assert(result);
	contrac_get_proximity_id_base64(contrac, rpi_base64);
	ck_assert_int_eq(strlen(rpi_base64), RPI_SIZE_BASE64);
	ck_assert_str_eq(rpi_base64, "yStiu899O+6xvdLiUdrpsA==");

	result = contrac_set_time_interval_number(contrac, 82);
	ck_assert(result);
	contrac_get_proximity_id_base64(contrac, rpi_base64);
	ck_assert_int_eq(strlen(rpi_base64), RPI_SIZE_BASE64);
	ck_assert_str_eq(rpi_base64, "aFTYIeEUGYKELi8TUUql+Q==");

	result = contrac_set_time_interval_number(contrac, 143);
	ck_assert(result);
	contrac_get_proximity_id_base64(contrac, rpi_base64);
	ck_assert_int_eq(strlen(rpi_base64), RPI_SIZE_BASE64);
	ck_assert_str_eq(rpi_base64, "MK9mDdgTsgsh6Vxp0XhasA==");

	contrac_set_tracing_key_base64(contrac, tracing_key_base64[1]);

	result = contrac_set_day_number(contrac, 500);
	ck_assert(result);

	result = contrac_set_time_interval_number(contrac, 1);
	ck_assert(result);
	contrac_get_proximity_id_base64(contrac, rpi_base64);
	ck_assert_int_eq(strlen(rpi_base64), RPI_SIZE_BASE64);
	ck_assert_str_eq(rpi_base64, "layNswS/rQxovfYnBhjXJg==");

	result = contrac_set_time_interval_number(contrac, 27);
	ck_assert(result);
	contrac_get_proximity_id_base64(contrac, rpi_base64);
	ck_assert_int_eq(strlen(rpi_base64), RPI_SIZE_BASE64);
	ck_assert_str_eq(rpi_base64, "BJyzAC3hyVfcWOhQ9paEFA==");

	result = contrac_set_time_interval_number(contrac, 143);
	ck_assert(result);
	contrac_get_proximity_id_base64(contrac, rpi_base64);
	ck_assert_int_eq(strlen(rpi_base64), RPI_SIZE_BASE64);
	ck_assert_str_eq(rpi_base64, "ItETUQc372fOzJKHvF8z6w==");

	// Clean up
	contrac_delete(contrac);
}
END_TEST

START_TEST (check_match) {
	bool result;
	char const *tracing_key_base64 = "3UmKrtcQ2tfLE8UPSXHb4PtgRfE0E2xdSs+PGVIS8cc=";
	RpiList * beacon_list;
	DtkList * diagnosis_list;
	// There are four matches in amongst this lot
	// (day, time) = (12, 15), (1175, 142), (1175, 67), (12, 93)
	uint32_t beacon_days[8] = {55, 12, 0, 8787, 1175, 1175, 187, 12};
	uint8_t beacon_times[8] = {1, 15, 5, 101, 142, 67, 51, 93};
	uint32_t diagnosis_days[2] = {1175, 12};
	// Summarise the four matches
	uint32_t match_days[4] = {12, 1175, 1175, 12};
	uint8_t match_times[4] = {15, 142, 67, 93};
	int pos;
	const unsigned char * rpi_bytes;
	const unsigned char * dtk_bytes;
	MatchList * matches;
	MatchListItem const * match;

	// Generate some keys, check the results
	Contrac * contrac;

	contrac = contrac_new();
	ck_assert(contrac != NULL);

	contrac_set_tracing_key_base64(contrac, tracing_key_base64);

	// Generate some beacons (as if collected over BlueTooth)

	beacon_list = rpi_list_new();

	for (pos = 0; pos < 8; ++pos) {
		result = contrac_set_day_number(contrac, beacon_days[pos]);
		ck_assert(result);

		result = contrac_set_time_interval_number(contrac, beacon_times[pos]);
		ck_assert(result);

		rpi_bytes = contrac_get_proximity_id(contrac);
		rpi_list_add_beacon(beacon_list, rpi_bytes, beacon_times[pos]);
	}

	// Generate some diagnosis data (as if provided by a diagnosis server)

	diagnosis_list = dtk_list_new();
	for (pos = 0; pos < 2; ++pos) {
		result = contrac_set_day_number(contrac, diagnosis_days[pos]);
		ck_assert(result);

		dtk_bytes = contrac_get_daily_key(contrac);
		dtk_list_add_diagnosis(diagnosis_list, dtk_bytes, diagnosis_days[pos]);
	}

	// Check that the matching algorithm identifies the beacons that match

	matches = match_list_new();
	match_list_find_matches(matches, beacon_list, diagnosis_list);

	match = match_list_first(matches);
	
	while (match) {
		result = false;
		for (pos = 0; pos < 4; ++pos) {
			if ((match_list_get_day_number(match) == match_days[pos]) && (match_list_get_time_interval_number(match) == match_times[pos])) {
				result = true;
			}
		}

		ck_assert(result);

		match = match_list_next(match);
	}
	ck_assert_int_eq(match_list_count(matches), 4);

	// Clean up
	match_list_delete(matches);
	rpi_list_delete(beacon_list);
	dtk_list_delete(diagnosis_list);
	contrac_delete(contrac);
}
END_TEST

START_TEST (check_time) {
	bool result;
	char const *tracing_key_base64 = "3UmKrtcQ2tfLE8UPSXHb4PtgRfE0E2xdSs+PGVIS8cc=";
	uint32_t dn;
	uint8_t tn;
	time_t base = 1587415596;
	time_t check;
	Contrac * contrac;
	char dtk_base64[DTK_SIZE_BASE64 + 1];
	char rpi_base64[RPI_SIZE_BASE64 + 1];

	check = base;
	dn = epoch_to_day_number(check);
	ck_assert_int_eq(dn, 18372);
	tn = epoch_to_time_interval_number(check);
	ck_assert_int_eq(tn, 124);

	check = base + (24 * 60 * 60);
	dn = epoch_to_day_number(check);
	ck_assert_int_eq(dn, 18372 + 1);
	tn = epoch_to_time_interval_number(check);
	ck_assert_int_eq(tn, 124);

	check = base - 675 * (24 * 60 * 60);
	dn = epoch_to_day_number(check);
	ck_assert_int_eq(dn, 18372 - 675);
	tn = epoch_to_time_interval_number(check);
	ck_assert_int_eq(tn, 124);

	check = base + 15 * (10 * 60);
	dn = epoch_to_day_number(check);
	ck_assert_int_eq(dn, 18372);
	tn = epoch_to_time_interval_number(check);
	ck_assert_int_eq(tn, 124 + 15);

	fake_time = 1587415596;

	contrac = contrac_new();
	contrac_set_tracing_key_base64(contrac, tracing_key_base64);

	contrac_update_current_time(contrac);
	result = contrac_get_initialised(contrac);
	ck_assert(result);

	contrac_get_daily_key_base64(contrac, dtk_base64);
	ck_assert_str_eq(dtk_base64, "6GMXmeDAR5XU0AEtZQJ+tA==");

	contrac_get_proximity_id_base64(contrac, rpi_base64);
	ck_assert_str_eq(rpi_base64, "ysbjVuF7xf9eDH2y2T8VpQ==");

	contrac_delete(contrac);
}
END_TEST

int main(void) {
	int failed;
	Suite * s;
	SRunner *sr;
	TCase * tc;

	s = suite_create("libcontrac");

	tc = tcase_create("Contrac");
	tcase_add_test(tc, check_base64);
	tcase_add_test(tc, check_contrac);
	tcase_add_test(tc, check_dtk);
	tcase_add_test(tc, check_rpi);
	tcase_add_test(tc, check_match);
	tcase_add_test(tc, check_time);
	suite_add_tcase(s, tc);
	sr = srunner_create(s);

	srunner_run_all(sr, CK_NORMAL);
	failed = srunner_ntests_failed(sr);
	srunner_free(sr);
	
	return (failed == 0) ? 0 : -1;
}

