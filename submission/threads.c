#include <stdio.h>
#include <stdlib.h>
#include <sysexits.h>
#include <pthread.h>
#include <unistd.h>
#include <sys/time.h>
#include <sys/resource.h>
#include "threads.h"

/* upper limit for array */
#define MAX_NUM 999

static int sum = 0;
static int max = 0;
/*
static pthread_mutex_t mutex;
*/

void verify_args(
	int *num_elements, int *num_threads, int *seed, int *task, char *print_results,
	int ne_arg, int nt_arg, int seed_arg, int task_arg, char pr_arg
);

struct timeval tv_delta(struct timeval start, struct timeval end) {
	struct timeval delta = end;
	delta.tv_sec -= start.tv_sec;
	delta.tv_usec -= start.tv_usec;
	if (delta.tv_usec < 0) {
		delta.tv_usec += 1000000;
		delta.tv_sec--;
	}
	return delta;
}

int main(int argc, char *argv[]) {
	int num_elements, num_threads, seed, task;
	char print_results;
	pthread_t *tids;
	Segment *seg;
	int i, *array;
	struct rusage start_ru, end_ru;
	struct timeval start_wall, end_wall;
	struct timeval delta_user, delta_system, delta_wall;

    /* input validation */
	if (argc < 6) {
		fprintf(
			stderr, 
			"Usage: %s num_elements num_threads seed task(1=max, 2=sum) print_results(Y/N)\n", 
			argv[0]
		);
		exit(EX_USAGE);
	}
	verify_args(
		&num_elements, &num_threads, &seed, &task, &print_results,
		atoi(argv[1]), atoi(argv[2]), atoi(argv[3]), atoi(argv[4]), argv[5][0]
	);

	printf("Number of Elements: %d\nNumber of Threads: %d\nSeed: %d\n", num_elements, num_threads, seed);
	if (task == 1) {
		printf("Task: get_max()\n");
	} else {
		printf("Task: get_sum()\n");
	}

	/*
	pthread_mutex_init(&mutex, NULL);
	*/
	array = malloc(sizeof(int) * num_elements);
	init_array(seed, num_elements, &array);
	/* make array of segs */
	seg = malloc(sizeof(Segment) * (num_threads + 1));
	for (i = 1; i <= num_threads; i++) {
		seg[i].array = array;
		seg[i].start = (i - 1) * (num_elements / num_threads);
		/* last thread may have different size if slicing is unequal */
		if (i == num_threads) {
			seg[i].size = num_elements - seg[i].start;
		} else {
			seg[i].size = num_elements / num_threads;
		}
	}

	/* time before thread creation/execution */
	getrusage(RUSAGE_SELF, &start_ru);
	gettimeofday(&start_wall, NULL);

	/* EXECUTING TASK ACROSS THREADS */
	tids = create_threads(num_elements, num_threads, task, seg);
	sync_threads(tids, num_threads);

	/* time after thread synchronization */
	getrusage(RUSAGE_SELF, &end_ru);
	gettimeofday(&end_wall, NULL);

	if (print_results == 'Y' || print_results == 'y') {
		if (task == 1) {
			printf("Result of get_max(): %d\n", max);
		} else {
			printf("Result of get_sum(): %d\n", sum);
		}
	}

	delta_wall = tv_delta(start_wall, end_wall);
	delta_user = tv_delta(start_ru.ru_utime, end_ru.ru_utime);
	delta_system = tv_delta(start_ru.ru_stime, end_ru.ru_stime);
	printf("Wall clock time: %f seconds\n", delta_wall.tv_sec + (delta_wall.tv_usec / (double) 1000000));
	printf("User time: %f seconds\n", delta_user.tv_sec + (delta_user.tv_usec / (double) 1000000));
	printf("System/kernel time: %f seconds\n", delta_system.tv_sec + (delta_system.tv_usec / (double) 1000000));

	/*
	pthread_mutex_destroy(&mutex);
	*/
	free(seg);
	free(array);
	free(tids);
	return 0;
}

void *get_max(void *arg) {
	Segment seg = *(Segment *) arg;
	int i, start = seg.start, end = seg.start + seg.size - 1;
	/*
	printf("get_max(): %d - %d, size: %d\n", start, end, seg.size);
	*/
	for (i = start; i <= end; i++) {
		/*
		pthread_mutex_lock(&mutex);
		*/
		if (max < seg.array[i]) {
			max = seg.array[i];
		}
		/*
		pthread_mutex_unlock(&mutex);
		*/	
	}
	return NULL;
}

void *get_sum(void *arg) {
	Segment seg = *(Segment *) arg;
	int i, start = seg.start, end = seg.start + seg.size - 1;
	/*
	printf("get_sum(): %d - %d, size: %d\n", start, end, seg.size);
	*/
	for (i = start; i <= end; i++) {
		/*
		pthread_mutex_lock(&mutex);
		*/
		sum += seg.array[i] % 1000000;
		/*
		pthread_mutex_unlock(&mutex);
		*/
	}
	return NULL;
}

int get_sum_expected(int size, int arr[]) {
	int i, s = 0;
	
	for (i = 0; i < size; i++) {
		s += arr[i] % 1000000;
	}
	return s;
}

int get_max_expected(int size, int arr[]) {
	int i, m = 0;

	for (i = 0; i < size; i++) {
		if (m < arr[i]) {
			m = arr[i];
		}
	}
	return m;
}

pthread_t *create_threads(
	int num_elements, int num_threads, int task, Segment seg[]
) {
	int i;
	pthread_t *tids;
	void *(*task_func)(void *arg);

	/* TODO: move to outside function call */
	tids = calloc(num_threads + 1, sizeof(pthread_t));
	if (task == 1) {
		task_func = get_max;
	} else {
		task_func = get_sum;
	}
	for (i = 1; i <= num_threads; i++) {
		pthread_create(&tids[i], NULL, task_func, &seg[i]);
	}
	return tids;
}

void sync_threads(pthread_t tids[], int num_threads) {
	int i;

	for (i = 1; i <= num_threads; i++) {
		pthread_join(tids[i], NULL);
	}
}

void init_array(int seed, int size, int **result) {
	int i;
	
	srand(seed);
	for (i = 0; i < size; i++) {
		(*result)[i] = rand() % (MAX_NUM + 1);
	}
}

void print_array(int size, int array[]) {
	int i;
	
	for (i = 0; i < size; i++) {
		printf("%d ", array[i]);
	}
	printf("\n");
}

void verify_args(
	int *num_elements, int *num_threads, int *seed, int *task, char *print_results,
	int ne_arg, int nt_arg, int seed_arg, int task_arg, char pr_arg
) {
	if ((*num_elements = ne_arg) <= 0) {
		fprintf(stderr, "num_elements: Invalid number. Must be above 0.\n");
		exit(EX_USAGE);
	}
	if ((*num_threads = nt_arg) > *num_elements || *num_threads < 0) {
		fprintf(stderr, "num_threads: Invalid number. Must be between 0 and %d.\n", *num_elements);
		exit(EX_USAGE);
	}
	*seed = seed_arg;
	if ((*task = task_arg) != 1 && *task !=2) {
		fprintf(stderr, "task: Invalid task. Must be 1 (max) or 2 (sum).\n");
		exit(EX_USAGE);
	}
	if ((*print_results = pr_arg) != 'Y' && *print_results != 'y' && 
		*print_results != 'N' && *print_results != 'n'
	) {
		fprintf(stderr, "print_results: Invalid option. Must be (Y/y)es or (N/n)o\n");
		exit(EX_USAGE);
	}
}