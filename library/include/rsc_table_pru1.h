#ifndef _RSC_TABLE_PRU1_H_
#define _RSC_TABLE_PRU1_H_

#include <stddef.h>
#include <rsc_types.h>
#include "pru.h"
#include "pru_virtio_ids.h"

struct my_resource_table {
	struct resource_table base;
	uint32_t offset[2]; /* Should match 'num' in actual definition */

};

#pragma DATA_SECTION(resourceTable, ".resource_table")
#pragma RETAIN(resourceTable)
struct my_resource_table resourceTable = {
	1,	/* we're the first version that implements this */
	0,	/* number of entries in the table */
	0, 0,	/* reserved, must be zero */
  0,
};

#endif /* _RSC_TABLE_PRU1_H_ */
