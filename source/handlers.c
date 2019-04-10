// Custom signal handlers
#include <stdio.h>
#include <unistd.h>
#include <signal.h>
extern int interrupted;

void interrupt_handler()
{
	printf("parent[%d]: Received keyboard interrupt\n", (int)getpid());
	interrupted = 1;	
}

void terminate_handler()
{
	printf("child[%d]: Received SIGTERM signal.\n", (int)getpid());
}

void set_all(void (*func)(int))
{
	for(int i=1; i<NSIG; i++)
		signal(i, func);
}
