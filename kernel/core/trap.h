#ifndef TRAP_H
#define TRAP_H

void init_trap_dispatch(void);
void trap_dispatch(uint64_t trapno, uint64_t err, uint64_t rip);

// System call numbers (example)
#define SYS_PUTS        0x01
#define SYS_GET_PID     0x02
// ... define more system calls as needed

typedef int (*syscall_handler_t)(uint64_t, uint64_t, uint64_t, uint64_t);

void register_syscall(uint64_t syscall_num, syscall_handler_t handler);

#endif
