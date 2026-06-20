#ifndef RSC_TABLE_H
#define RSC_TABLE_H
#include <stdint.h>
struct ow_rsc { uint32_t ver, num, reserved[2]; };
__attribute__((section(".resource_table"), used))
const struct ow_rsc ow_resource_table = { 1u, 0u, { 0u, 0u } };
#endif
