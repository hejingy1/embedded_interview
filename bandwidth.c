#define _GNU_SOURCE
#ifndef _TIME_UTIL_H_
#define _TIME_UTIL_H_

#include <assert.h>
#include <sched.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>
#include <sys/sysinfo.h>
// #include "time_util.h"
#include <inttypes.h>

#define KB 1024
#define MB 1024 * 1024

static inline struct timespec difftimespec(struct timespec t1, struct timespec t0)
{
	assert(t1.tv_nsec < 1000000000);
	assert(t0.tv_nsec < 1000000000);

	return (t1.tv_nsec >= t0.tv_nsec)
		? (struct timespec){ t1.tv_sec - t0.tv_sec    , t1.tv_nsec - t0.tv_nsec             }
		: (struct timespec){ t1.tv_sec - t0.tv_sec - 1, t1.tv_nsec - t0.tv_nsec + 1000000000};
}

// Convert struct timespec to seconds
static inline double timespec_to_sec(struct timespec t)
{
	return t.tv_sec * 1.0 + t.tv_nsec / 1000000000.0;
}

// Convert struct timespec to nanoseconds
static inline double timespec_to_nsec(struct timespec t)
{
	return t.tv_sec * 1000000000.0 + t.tv_nsec / 1.0;
}
#endif// _TIME_UTIL_H_

void memoryBandwith(){
	// Method: write to an array to see the memory speed and hit ratio
	// The testing output will be stored inside file memory.txt
	FILE * fp;
	struct timespec t0, t1, time_diff;
	int n=2000000, tests = 10;
	

	fp = fopen ("mem_data.txt", "w+");
	
	for(int test=0; test<tests; test++){

	
		int arr[n];

		clock_gettime(CLOCK_MONOTONIC, &t0);
		for(int i=0; i<n; i++){
			arr[i]=i;
		}
		clock_gettime(CLOCK_MONOTONIC, &t1);

		arr[1]= arr[2];
		time_diff = difftimespec(t1, t0);
		double time_sec = timespec_to_sec(time_diff);

		fprintf(fp,"%f,%0.2f,%d\n",time_sec, ((sizeof(int)*n)/1000000)/time_sec, test);
	}
	fclose(fp);
}

static void cacheTest1(){
	// Method: write to array in increments to test the jumps of each cache level
	// The testing output will be stored inside file cache_data.txt

	FILE * fp;

	fp = fopen ("cache_data.txt", "w+");

	int m = 2000 *MB;
	char *heapArr = (char*) malloc(m);

	int temp,step=2, iters=0, lim=100;
	struct timespec t2, t3, time_diff2;
	double sum = 0, time_sec;

	for(int j = step; j<15*MB;j*=1.5){
		sum = 0;
		for(int x=0; x<100; x++){
		clock_gettime(CLOCK_MONOTONIC, &t2);	
		for(int i=1; i<m;i+= j){
			if(iters == lim){break;}
			heapArr[i]=i;
			iters++;
		}
		clock_gettime(CLOCK_MONOTONIC, &t3);
		time_diff2 = difftimespec(t3, t2);
		time_sec = timespec_to_nsec(time_diff2);
		sum += time_sec/lim;
		temp = 0;
		}
		fprintf(fp,"%.2f,%d,%d,%d\n",sum/100,j,m/1000, iters);
		iters = 0;
	}

	temp = temp+1;
	free(heapArr);
	fclose(fp);

}



int main(int argc, char *argv[])
{
	// Random generator based on current timestamp.
	srandom(time(NULL));
	cpu_set_t set;
	CPU_ZERO(&set);
	CPU_SET((random() ?: 1) % get_nprocs(), &set);
	if (sched_setaffinity(getpid(), sizeof(set), &set) != 0) {
		perror("sched_setaffinity");
		return 1;
	}

	memoryBandwith();
	cacheTest1();

}
