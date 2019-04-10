// Custom signal handlers
#include <stdio.h>
#include <unistd.h>
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
