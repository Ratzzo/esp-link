#ifndef TRANSITIONS_H
#define TRANSITIONS_H

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

//t is incremented linearly in the range of t_start to t_end in each tick, it's up to the formula to define the behavior
typedef int (*transition_tick_cb_t)(int t);


typedef struct transition_t {
	uint8_t state; //1: running, 0: standing_by;
	int t; //current t
	transition_tick_cb_t formula;
	int t_start; //initial t
	int t_end; //final t
} transition_t;

int transition_create(transition_t *tr, transition_tick_cb_t tick_cb);
void transition_begin(transition_t *tr, int t_start, int t_end);
void transition_advance(transition_t *tr);

#ifdef __cplusplus
}
#endif

#endif
