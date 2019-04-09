#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>

#define WITH_SIGNALS
#define NUM_CHILD 60

#ifdef WITH_SIGNALS
int interrupted = 0;

void interrupt_handler();
void terminate_handler();
#endif

void terminate_children(int amount, int* pids);

int main(int argc, char** argv)
{
	#ifdef WITH_SIGNALS
	// Ignore all signals
	for(int i=1; i<NSIG; i++)
		signal(i, SIG_IGN);
	// Restore SIGCHLD
	signal(SIGCHLD, SIG_DFL);
	// Set custom interrupt handler
	signal(SIGINT, interrupt_handler);
	#endif
	pid_t child_pids[NUM_CHILD];
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
			// Add to PID array
			child_pids[i] = pid;
			printf("parent[%d]: Child process created. PID: %d\n", (int)getpid(), (int)child_pids[i]);
			++children_count;

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
	for(int i=1; i<NSIG; i++)
		signal(i, SIG_DFL);
	#endif
	return 0;
}

void terminate_children(int amount, int* pids)
{
	for(int j=0; j<amount; j++)
	{
		kill(pids[j], SIGTERM);
		printf("parent[%d]: Sending SIGTERM to PID %d\n", (int)getpid(), (int)pids[j]);
	}
}

#ifdef WITH_SIGNALS
void interrupt_handler()
{
	printf("parent[%d]: Received keyboard interrupt\n", (int)getpid());
	interrupted = 1;	
}

void terminate_handler()
{
	printf("child[%d]: Received SIGTERM signal.\n", (int)getpid());
}
#endif
