#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <stdbool.h>

#define PROC_TABLE_SZ 18

typedef struct{
	unsigned long seconds;
	unsigned long nanoSec;	
}Clock;

typedef struct{

	int pid;
	int localPid;
	bool realTime;
	unsigned int timeQuantum;
	Clock cpuTimeUsed;
	Clock timeInSys;
	unsigned long timeLastBurst;
	Clock blockedTime;
	Clock unblockedTime;
	Clock scheduledTime;
	Clock finishedTime;
}PCBt;

typedef struct{
	PCBt pcb[PROC_TABLE_SZ];
	Clock sysTime; 

}AllS;

int sharedMemory();
void *attachSharedMemory(int shmid, unsigned int readonly);
void incrementClock(struct Clock *clock, int increment);
struct Clock addClock(struct Clock clk1, struct Clock clk2);
int compareClock(struct Clock clk1, struct Clock clk2);
long double clockSeconds(struct Clock clk);
struct Clock secondsClock(long double seconds);
struct Clock calculateATime(struct Clock clk, int divisor);
struct Clock subtractClock(struct Clock clk1, struct Clock clk2);
void printClock(char *name, struct Clock clck);

#endif
