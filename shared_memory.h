/*
	By:Michael Sago
	4/16/19
*/
#ifndef SHARED_MEMORY_H
#define SHARED_MEMORY_H

#define PROC_TABLE_SZ 18
#define ONE_BILL 1000000000

const unsigned int SYSCLOCK_ID_IDX = 1; 
const unsigned int PCT_ID_IDX = 2; 
const unsigned int MAX_BETWEEN_PROC = 2000000000;
const unsigned B_TIME_QUANTUM = 10000000;

typedef struct{
	unsigned long seconds;
	unsigned long nanoSec;	
}Clock;

typedef struct{
	int pid;
	int localPid;
	int realTime;
	unsigned int timeQuantum;
	Clock cpuTimeUsed;
	Clock timeInSys;
	unsigned long timeLastBurst;
	Clock blockedTime;
	Clock waitTime;
}PCB;

typedef struct{
	PCB pcbs[PROC_TABLE_SZ];
	Clock sysTime; 

}PCBTable;


#endif
