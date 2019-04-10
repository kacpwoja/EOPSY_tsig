#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>

#include <defines.h>

#ifdef WITH_SIGNALS
#include <handlers.h>

int interrupted = 0;
#endif

#ifdef DYNAMIC
#include <stdlib.h>
#endif

void terminate_children(int amount, pid_t* pids);

int main(int argc, char** argv)
{
	#ifdef WITH_SIGNALS
	// Ignore all signals
	set_all(SIG_IGN);
	// Restore SIGCHLD
	signal(SIGCHLD, SIG_DFL);
	// Set custom interrupt handler
	signal(SIGINT, interrupt_handler);
	#endif

	// Allocating array for PIDs	
	#ifndef DYNAMIC
	pid_t child_pids[NUM_CHILD];
	#endif
	#ifdef DYNAMIC
	pid_t* child_pids = (pid_t*)malloc(sizeof(pid_t));
	if(child_pids == NULL)
	{
		printf("parent[%d]: Could not allocate memory. Terminating all children and exiting.\n", (int)getpid());
		return 2;
	}
	#endif

	int children_count = 0;
	pid_t pid;

	for(int i=0; i<NUM_CHILD; i++)
	{
		#ifdef WITH_SIGNALS
		// Check interrupt
		if(interrupted != 0)
		{
			// If process interrupted, print message and exit loop
			terminate_children(i, child_pids);
			printf("parent[%d]: Child creation interrupted.\n", (int)getpid());
			break;
		}
		#endif
		// Create child process
		pid = fork();

		// Fork unsuccessful
		if(pid < 0)
		{
			// Send SIGTERM to all already forked children
			terminate_children(i, child_pids);
			// And exit with error code 1
			return 1;
		}
		
		// In Parent Process
		if(pid > 0)
		{
			++children_count;
			#ifdef DYNAMIC
			// Resize PID array
			child_pids = realloc(child_pids, children_count*sizeof(pid_t));
			if(child_pids == NULL)
			{
				printf("parent[%d]: Could not allocate memory. Terminating all children and exiting.\n", (int)getpid());
				return 2;
			}
			#endif
			// Add to PID array
			child_pids[i] = pid;
			printf("parent[%d]: Child process created. PID: %d\n", (int)getpid(), (int)child_pids[i]);

			// Wait for 1 second
			sleep(1);
			continue;
		}

		// In Child Process
		if(pid == 0)
		{
			#ifdef WITH_SIGNALS
			signal(SIGINT, SIG_IGN);
			signal(SIGTERM, terminate_handler); 
			#endif
			printf("child[%d]: PID of parent process: %d\n", (int)getpid(), (int)getppid());
			sleep(10);
			printf("child[%d]: Completed execution: exiting with code 0\n", (int)getpid());
			return 0;
		}
	}
	int exit_codes[children_count];	
	int finished = 0;
	int code;
	while(finished < children_count)
	{
		// Wait for child to exit
		pid = wait(&code);

		// Save exit code
		for(int i=0; i<children_count; i++)
		{
			if(child_pids[i] == pid)
			{
				exit_codes[i] = code;
				break;
			}
		}

		++finished;
	}

	// Print ending message and all exit codes
	printf("parent[%d]: No more children remaining. List of all exit codes:\n", (int)getpid());
	for(int i=0; i<children_count; i++)
	{
		printf("PID: %d Exit Code: %d\n", child_pids[i], exit_codes[i]);
	}

	#ifdef WITH_SIGNALS
	// Return signal handlers
	set_all(SIG_DFL);
	#endif

	#ifdef DYNAMIC
	// Free memory
	free(child_pids);
	#endif
	return 0;
}

void terminate_children(int amount, pid_t* pids)
{
	for(int j=0; j<amount; j++)
	{
		kill(pids[j], SIGTERM);
		printf("parent[%d]: Sending SIGTERM to PID %d\n", (int)getpid(), (int)pids[j]);
	}
}
