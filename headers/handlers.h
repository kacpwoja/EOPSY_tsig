#ifndef HANDLERS
#define HANDLERS

void interrupt_handler();
void terminate_handler();
void set_all(void (*func)(int));

#endif
