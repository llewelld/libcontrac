# libcontrac ReadMe

libcontrac is an implementation of the Contact Tracing API.

See the draft specs: https://www.apple.com/covid19/contacttracing/

## Install

If you have autoconf you can install as follows.

```
autoconf --install
./configure
make
make check
make install
```

## Usage

Include header files.
```
#include "contrac/contrac.h"
#include "contrac/rpi.h"
#include "contrac/dtk.h"
#include "contrac/rpi_list.h"
#include "contrac/rpi_list.h"
#include "contrac/matches.h"
```

### Broadcasting and uploading keys

Most of the functionality revolves around the opaque `Contrac` structure.

Create and initialise the structure as follows. The day and interval number
should be set appropriately.

```
Contrac * contrac = contrac_new();
contrac_generate_tracing_key(contrac);
contrac_set_day_number(contrac, 23);
contrac_set_time_interval_number(contrac, 76);
```

Get the Rolling Proximity Identifier for broadcast in Bluetooth beacons.
```
// Returns a buffer containing RPI_SIZE bytes of data
const unsigned char * rpi = contrac_get_proximity_id(contrac);
```

Get the Daily Tracing Key to upload to a Diagnosis Server in case of a positive
test result.
```
// Returns a buffer containing DTK_SIZE bytes of data
const unsigned char * dtk = contrac_get_daily_key(contrac);
```

### Receiving and matching keys

Add RPIs captured via Bluetooth to an RPI list.
```
RpiList * rpis = rpi_list_new();

// Add bytes captured at a given time to the list
Rpi * rpi = rpi_new();
rpi_assign(rpi, captured_bytes, time_interval_number);
rpi_list_append(rpis, rpi);
```

Construct a list of DTKs from data downloaded from a Diagnosis Server.
```
DtkList * dtks = dtk_list_new();

// Add data downloaded from the server to the list
Dtk * dtk = dtk_new();
dtk_assign(dtk, dtk_bytes, day_number);
dtk_list_append(dtks, dtk);
```

Having collected these two lists any matches can be tested for as follows. 

```
MatchList * matches = match_list_new();
match_list_find_matches(matches, rpis, dtks);

if (match_list_count(matches) > 0) {
	printf("Exposure identified, follow medical advice\n");
}
```

### Cleaning up

Finally, clean up.
```
match_list_delete(matches);
rpi_list_delete(rpis);
dtk_list_delete(dtks);
contrac_delete(contrac);
```

## Licence

Copyright (c) David Llewellyn-Jones, 2020

libcontrac is released under the GPL v2 Licence. Read COPYING for information on the license.

## Contact and Links

More information can be found at: https://www.flypig.co.uk/contrac

The source code can be obtained from git:
```
git clone git@www.flypig.org.uk:libcontrac
```

I can be contacted via one of the following.

 * My website: https://www.flypig.co.uk
 * Email: at david@flypig.co.uk

