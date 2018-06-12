#include <stdbool.h>
#include <semaphore.h>
#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <errno.h>
#include <string.h>
#include <unistd.h>
//#define N 2 // number of threads


int a = 0;
int size = 0;
int position = 0;

volatile int Nt=2;
volatile int Mi=2;
volatile int buffer[1000];

pthread_mutex_t mtx = PTHREAD_MUTEX_INITIALIZER; // for buffer access
sem_t semaphore; // controls the pool of threads


static void * worker(void *arg) {
	// input
	int j;
   	while (true) {
	   // block until work is available
	   if (sem_wait(&semaphore)) {
	      fprintf(stderr, "sem_wait: %s\n", strerror(errno));
	      exit(EXIT_FAILURE);
	   }
	   // get exclusive access to buffer
	   if (pthread_mutex_lock(&mtx)) {
	      fprintf(stderr, "pthread_mutex_lock: %s\n", strerror(errno));
	      exit(EXIT_FAILURE);
	   }
	   // store input
	   j = buffer[position];
		 position++;
	   // release exclusive access to buffer
	   if (pthread_mutex_unlock(&mtx)) {
	      fprintf(stderr, "pthread_mutex_unlock: %s\n", strerror(errno));
	      exit(EXIT_FAILURE);
	   }
	   // process data
	   fprintf(stdout, "Processing: %d\n", j);
	   sleep(10);
		 a--;
	   fprintf(stdout, "Done with: %d\n", j);
	}
}

int main(int argc, char **argv) {
	// input
	int j;
	//input at specific positions to take in as threat and number of items
	Nt = atoi(argv[1]);
	Mi = atoi(argv[2]);
	a = a-Nt;
	fprintf(stdout, "*** %d threads, %d buffered items\n", Nt, Mi);

	// initialize semaphore
	if (sem_init(&semaphore, 0, 0)) {
	      fprintf(stderr, "sem_init: %s\n", strerror(errno));
	      exit(EXIT_FAILURE);
	}
	// create pool of N detached threads
   	pthread_t t;
  	for (int i=0; i<Nt; ++i) {
	   if (pthread_create(&t, NULL, worker, NULL)) {
	      fprintf(stderr, "pthread_create: %s\n", strerror(errno));
	      exit(EXIT_FAILURE);
	   }
	   if (pthread_detach(t)) {
	      fprintf(stderr, "pthread_detach: %s\n", strerror(errno));
	      exit(EXIT_FAILURE);
	   }
   	}
	// main loop
   	while (fscanf(stdin, "%d", &j)!=EOF) {
	   // get exclusive access to buffer
		 //check to take in input or ignore it
		 if(a<Mi){
			 //if(Mi >)
			 //for(int t=0;){
			 //  arg

		   if (pthread_mutex_lock(&mtx)) {
		      fprintf(stderr, "pthread_mutex_lock: %s\n", strerror(errno));
		      exit(EXIT_FAILURE);
		   }
		   // load input into buffer
		   buffer[size] = j;
			 size++;
			 a++;
		   // release exclusive access to buffer
		   if (pthread_mutex_unlock(&mtx)) {
		      fprintf(stderr, "pthread_mutex_unlock: %s\n", strerror(errno));
		      exit(EXIT_FAILURE);
		   }
		   // unlock one thread
		   if (sem_post(&semaphore)) {
		      fprintf(stderr, "sem_post: %s\n", strerror(errno));
		      exit(EXIT_FAILURE);
		   }
	 	}
		//buffer is full
		else{
			fprintf(stdout, "buffer reached full capacity!\n");
		}
	}
	return(0);
}
