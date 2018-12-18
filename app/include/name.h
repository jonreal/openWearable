#ifndef _NAME_
#define _NAME_

#include "fix16.h"

typedef struct {
 char* name;
 uint32_t size;
 char print_type;
} name_size_t;

const uint32_t num_state = 2;
const name_size_t state_name_size[2] =
 { {"time",sizeof(uint32_t),"u"},
   {"p_m",sizeof(fix16_t),"f"}
 };

#endif
