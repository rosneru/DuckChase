#ifndef TIMER_FUNCTIONS_H
#define TIMER_FUNCTIONS_H

#include <exec/types.h>

BOOL InitializeTimer();
void FreeTimer();
void StartTimer();


/**
 * Picks the elapsed time.
 *
 * @param p_bKeepStartPoint
 * When true, the timer (stop watch) is not reset when picking.
 * That means that the former start time will be used at next pick
 * again.
 *
 * @returns
 * Elapsed time in milliseconds or -1.0 if an error occurred
 */
double PickTime(BOOL bKeepStartPoint);

#endif

