#ifndef _FORMAT_H_
#define _FORMAT_H_

#include "mem_types.h"

void FormatSprintParams(const param_mem_t* param, char* buff);
void FormatSprintState(const state_t* st, char* buff);
void FormatSprintStateHeader(char* buff);
void FormatSprintPublishState(const state_t* st, char* buff);

#endif
