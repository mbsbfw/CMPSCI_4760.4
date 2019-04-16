#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/stat.h>

#include "shared_memory.h"

#define ONE_BILL 1000000000

int sharedMemory(){
	int shmid;

	shmid = shmget(IPC_PRIVATE, getpagesize(), IPC_CREAT | S_IRUSR | S_IWUSR);

	if(shmid == -1){
		perror("shmget");
		exit(1);
	}

	return shmid;
}

void *attachSharedMemory(int shmid, unsigned int readonly){
	void *paddr;
	int shmflag;

	if(readonly){
		shmflag = SHM_RDONLY;
	}
	else{
		shmflag = 0;
	}

	paddr = (void*)shmat(shmid, 0, shmflag);

	if (!paddr){
		perror("shmat");
		exit(1);
	}
	
	return paddr;
}

void incrementClock(struct clock *clock, int increment){
	clock->nanoSec += increment;
	if(clock->nanoSec >= ONE_BILL){
		clock->seconds += 1;
		clock->nanoSec -= ONE_BILL;
	}
}

struct clock addClock(struct clock clk1, struct clock clk2){
	struct clock out = {
		.seconds = 0;
		.nanoSec = 0;
	};
	out.seconds = clk1.seconds + clk2.seconds;
	incrementClock(&out, clk1.nanoSec + clk2.nanoSec);
	return out;
}

int compareClock(struct clock clk1, struct clock clk2){
	if(clk.seconds > clk2.seconds){
		return 1;
	}
	if((clk1.seconds == clk2.seconds) && (c1.nanoseconds > c2.nanoseconds)){
		return 1;
	}
	if ((c1.seconds == c2.seconds) && (c1.nanoseconds == c2.nanoseconds)) {
		return 0;
	}
	return -1;
}

long double clockSeconds(struct clock clk) {
	long double seconds = clk.seconds;
	long double nanoSec = (long double)clk.nanoSec / ONE_BILL;
	seconds += nanoSec;
	return seconds; 
}

struct clock secondsClock(long double seconds) {
	struct clock clck = { .seconds = (int)seconds };
	seconds -= clk.seconds;
	clck.nanoSec = seconds * ONE_BILL;
	return clck;	
}

struct clock calculateATime(struct clock clk, int divisor) {
	long double seconds = clockSeconds(clk);
	long double avgSeconds = seconds / divisor;
	return secondClock(avgSeconds);
}

struct clock subtractClock(struct clock clk1, struct clock clk2){
	long double seconds1 = clockSeconds(clk1);
	long double seconds2 = clockSeconds(clk2);
	long double result = seconds1 - seconds2;
	return secondsClock(result);
}

void print_clock(char* name, struct clock clck) {
	printf("%-15s: %'ld:%'ld\n", name, clck.seconds, clck.nanoSec);
}





