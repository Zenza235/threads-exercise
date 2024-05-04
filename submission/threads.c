#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <pthread.h>
#include <unistd.h>

/* global sum var? */
/* make sum func reentrant? */

int main(int argc, char *argv[]) {
	int invalid_input = 0;
	int num_elements, num_threads, seed, task;
	char print_results;

	/* verifying arguments */
	if (argc < 6) {
		fprintf(
			stderr, 
			"Usage: %s num_elements num_threads seed task(1=max, 2=sum) print_results(Y/N)\n", 
			argv[0]
		);
		exit(EX_USAGE);
	}
	if ((num_elements = atoi(argv[1])) <= 0) {
		fprintf(stderr, "num_elements: Invalid number. Must be above 0.\n");
		invalid_input = 1;
	}
	if ((num_threads = atoi(argv[2])) > num_elements || num_threads < 0) {
		fprintf(stderr, "num_threads: Invalid number. Must be between 0 and %d.\n", num_elements);
		invalid_input = 1;
	}
	seed = atoi(argv[3]);
	if ((task = atoi(argv[4])) != 1 && task !=2) {
		fprintf(stderr, "task: Invalid task. Must be 1 (max) or 2 (sum).\n");
		invalid_input = 1;
	}
	if ((print_results = argv[5][0]) != 'Y' && print_results != 'y' && 
		print_results != 'N' && print_results != 'n'
	) {
		fprintf(stderr, "print_results: Invalid option. Must be (Y/y)es or (N/n)o\n");
		invalid_input = 1;
	}
	if (invalid_input) {
		exit(EX_USAGE);
	}
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
	printf(
		"Number of Elements: %d\nNumber of Threads: %d\nSeed: %d\nTask: %d\nPrint results? %c\n", 
		num_elements, num_threads, seed, task, print_results
	);
	return 0;
}

/*
	Calculates max for *array segment*
*/
void *get_max(void *arg) {

}

void *get_sum(void *arg) {

}