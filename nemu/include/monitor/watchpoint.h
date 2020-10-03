#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;

	/* TODO: Add more members if necessary */
	char eexpression[100];
  	uint32_t init;

} WP;

WP* new_watchpoint();
void free_watchpoint(int N);
void show_watchpoint();
bool check_watchpoint();
#endif
