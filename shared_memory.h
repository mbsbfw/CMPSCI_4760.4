#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#include <stdbool.h>

#define PROC_TABLE_SZ 18
#define ONE_BILL 1000000000

#define CLOCK_KEY 314159
#define PCB_KEY 3141592
#define MSG_QUEUE 31415926

#define TIME_QUANTUM 10
#define MSGSZ 50

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
}PCB;

typedef struct{
	PCB pcbs[PROC_TABLE_SZ];
	Clock sysTime; 

}processCtrlTable;

struct msgbuf {
	long mtype;
	char mtext[MSGSZ];
};

void attachSharedMemory();

//void incrementClock(struct Clock *clock, int increment);
//struct Clock addClock(struct Clock clk1, struct Clock clk2);
//int compareClock(struct Clock clk1, struct Clock clk2);
//long double clockSeconds(struct Clock clk);
//struct Clock secondsClock(long double seconds);
//struct Clock calculateATime(struct Clock clk, int divisor);
//struct Clock subtractClock(struct Clock clk1, struct Clock clk2);
//void printClock(char *name, struct Clock clck);

#endif
