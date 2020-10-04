#ifndef __WATCHPOINT_H__
#define __WATCHPOINT_H__

#include "common.h"

typedef struct watchpoint {
	int NO;
	struct watchpoint *next;

	/* TODO: Add more members if necessary */
	char expression[100];
  	swaddr_t address;
	swaddr_t value; 

} WP;

WP* new_watchpoint();
void free_watchpoint(int watchpointNO);
void show_watchpoint();
bool check_watchpoint();
#endif
