#ifndef INCLUDE_TIMER_H
#define INCLUDE_TIMER_H

void timer_handler(struct cpu_state * r);
void timer_wait(int ticks);

#endif