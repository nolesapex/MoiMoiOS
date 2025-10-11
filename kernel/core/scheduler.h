#ifndef SCHEDULER_H
#define SCHEDULER_H

#include "types.h"

// Basic scheduler initialization function
bool init_scheduler();

// Function to start the scheduler (e.g., enable interrupts and switch to first task)
void scheduler_start();

#endif // SCHEDULER_H
