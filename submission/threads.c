#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <pthread.h>
#include <unistd.h>

/* global sum var? */
/* make sum func reentrant? */

int main(int argc, char *argv[]) {
	/*
		Arguments:
		- # of elements
		- # of threads
		- rng seed
		- task (1 -> max, 2 -> sum)
		- print results (Y/N)
	*/
	int num_elements, num_threads, seed, task;
	char print_results;

	if (argc < 6) {
		fprintf(stderr, "Usage: %s <num_elements> <num_threads> <seed> <task (1=max, 2=sum)> <print_results (Y/N)>\n", argv[0]);
		exit(EX_USAGE);
	}
	num_elements = atoi(argv[1]);
	num_threads = atoi(argv[2]);
	seed = atoi(argv[3]);
	task = atoi(argv[4]);
	print_results = argv[5][0];
	/*
		Create array of thread ids; don't assign w/ integer
		because threads may start executing at random times and
		assign themselves the wrong id because (e.g. P2 starts before
		P1 and now both have id = 2)

		(locking)
		init lock before creating threads (pthread_mutex_init)
		before modifying value shared between threads, lock
		after modifying value, unlock
		destroy lock when ending main process
	*/
	printf("Number of Elements: %d\nNumber of Threads: %d\nSeed: %d\nTask: %d\nPrint results? %c\n", num_elements, num_threads, seed, task, print_results);
	return 0;
}

/*
	Calculates max for *array segment*
*/
void *get_max(void *arg) {

}

void *get_sum(void *arg) {

}