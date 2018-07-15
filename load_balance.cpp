#include <stdlib.h>
#include <pthread.h>
#include <stdio.h>
#include <time.h>
#include <unistd.h>

#define SIZE 100
int theArray[SIZE];
pthread_mutex_t lock; 

typedef struct _bounds{
	int id, start, end;
} bounds;

void printArray(int *theArray){
	for (int i = 0; i < 100; i++){
		printf("%d:[%d] ", i, theArray[i]);
	}	
	printf("\n");
}

void *doubleTheValue(void *args){
	bounds *threadBounds = (bounds *)args;
	printf("Worker %d will work on array[%d,%d]\n", threadBounds->id, threadBounds->start, threadBounds->end-1);
	for(int k = threadBounds->start; k < threadBounds->end; k++){
	    sleep(rand()%2);
 	    pthread_mutex_lock(&lock);
	    //printf("Thread %d now in critical section.  ", threadBounds->id);
	    theArray[k] *= 2;
	    pthread_mutex_unlock(&lock);
	}
 	printf("***Thread %d is complete!\n", threadBounds->id);
	pthread_exit(NULL); 
}

void load_balancer(int numThreads){
	if (numThreads < 1) { return; }

	int rc;
	bounds threadBounds[SIZE];
	pthread_t threads[numThreads];
	pthread_mutex_init(&lock, NULL);

	srand(time(NULL)); //from https://stackoverflow.com/questions/822323/how-to-generate-a-random-int-in-c/39475626#39475626
	
 	//Initialize array to random values
	for (int i = 0; i < 100; i++){
		theArray[i] = rand() % 100 + 1;	//Set element to a random int from 0 to 99
                                    //from https://www.programmingsimplified.com/c-program-generate-random-numbers
	}	

	//Print the array
	printArray(theArray);
	
	int boundsLength = SIZE / numThreads;
	//do threading
	for (int k = 0; k < numThreads; k++){
    	threadBounds[k].id = k;
		threadBounds[k].start = boundsLength * k;
		threadBounds[k].end = threadBounds[k].start + boundsLength;
		rc = pthread_create(&threads[k], NULL, &doubleTheValue, &threadBounds[k]);
    }

	//Wait to rejoin all threads
	for (int i = 0; i < numThreads; i++){
		pthread_join(threads[i], NULL);
	}

	printArray(theArray);
}

int main(int argc, char *argv[]) {
	int numThreads = 4;
	if (argc>1) { numThreads = atoi(argv[1]); }
	if (numThreads <= 0){ numThreads = 4; } //Default Value if none given ... dbl check arg wasnt negative
	printf("Num threads:%d\n", numThreads);
	load_balancer(numThreads);

	return 0;
}
