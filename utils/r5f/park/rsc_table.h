#ifndef RSC_TABLE_H
#define RSC_TABLE_H
#include <stdint.h>
/* Empty resource table (num=0). The K3 driver maps the DT carveout + requests the
 * DT mailbox itself; we use no rpmsg vrings, so no VDEV entry is needed. */
struct ow_rsc { uint32_t ver, num, reserved[2]; };
__attribute__((section(".resource_table"), used))
const struct ow_rsc ow_resource_table = { 1u, 0u, { 0u, 0u } };
#endif
