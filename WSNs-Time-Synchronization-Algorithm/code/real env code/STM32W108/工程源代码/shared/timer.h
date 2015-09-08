#ifndef TIMER_H
#define TIMER_H


/* This timer can use SysTick */
u32 TIME_CurrentTime();

u32 TIME_ELAPSED(u32 startTime);

void TIMER_Init(void);

void TIMER_Tick(void);

#endif //TIMER_H
