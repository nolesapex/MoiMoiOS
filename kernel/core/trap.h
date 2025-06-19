#ifndef TRAP_H
#define TRAP_H

void init_trap_dispatch(void);
void trap_dispatch(uint64_t trapno, uint64_t err, uint64_t rip);

#endif
