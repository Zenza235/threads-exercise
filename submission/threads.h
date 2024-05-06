#include <pthread.h>

typedef struct {
	int *array, start, size;
} Segment;

void *get_max(void *arg);
void *get_sum(void *arg);
int get_sum_expected(int size, int arr[]);
int get_max_expected(int size, int arr[]);
pthread_t *create_threads(long num_elements, int num_threads, int task, Segment seg[]);
void sync_threads(pthread_t tids[], int num_threads);
void init_array(int seed, int size, int **result);
void print_array(int size, int array[]);
