#include <stdio.h>
#include <unistd.h>
#include <sys/types.h>
#include <signal.h>
#include <sys/wait.h>

//#define WITH_SIGNALS
#define NUM_CHILD 5

int main(int argc, char** argv)
{
	pid_t child_pids[NUM_CHILD];
	pid_t pid;

	for(int i=0; i<NUM_CHILD; i++)
	{
		// Create child process
		pid = fork();

		// Fork unsuccessful
		if(pid < 0)
		{
			// Send SIGTERM to all already forked children
			for(int j=0; j<i; j++)
			{
				kill(child_pids[j], SIGTERM);
				printf("parent[%d]: Sending SIGTERM to PID %d\n", (int)getpid(), (int)child_pids[j]);
			}
			// And exit with error code 1
			return 1;
		}
		
		// In Parent Process
		if(pid > 0)
		{
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
			printf("child[%d]: PID of parent process: %d\n", (int)getpid(), (int)getppid());
			sleep(10);
			printf("child[%d]: Completed execution: exiting with code 0\n", (int)getpid());
			return 0;
		}
	}
	int exit_codes[NUM_CHILD];	
	int finished = 0;
	int code;
	while(finished < NUM_CHILD)
	{
		// Wait for child to exit
		pid = wait(&code);

		// Save exit code
		for(int i=0; i<NUM_CHILD; i++)
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
	printf("parent[%d]: No more children remaining.List of all exit codes:\n", (int)getpid());
	for(int i=0; i<NUM_CHILD; i++)
	{
		printf("PID: %d Exit Code: %d\n", child_pids[i], exit_codes[i]);
	}

	return 0;
}
