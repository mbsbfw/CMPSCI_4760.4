#include <stdio.h>                                                              
#include <unistd.h>                                                             
#include <string.h>                                                             
#include <sys/wait.h>                                                           
#include <sys/time.h>                                                           
#include <time.h>                                                               
#include <sys/types.h>                                                          
#include <sys/ipc.h>                                                            
#include <sys/msg.h>                                                            
#include <sys/queue.h>
#include <sys/shm.h> 
#include <sys/stat.h>                                                           
#include <errno.h>     

#include "shared_memory.h"

#define MSGSZ 50

int simClockId;
int pcbTableId;
Clock *sysclock;
PCBTable *pct;

struct msgbuf {
	long mtype;
	char mtext[MSGSZ];
}msgbuf;


int getSharedMemory();
void* attachSharedMemory(int shmid, unsigned int readonly);

int main(int argc, char *argv[]){

	//IDs
	int sysClock_ID = atoi(argv[SYSCLOCK_ID_IDX]);
	sysclock = attachSharedMemory(sysClock_ID, 1);
 	
	//Attach shared memory
	int pcbTable_Id = atoi(argv[PCT_ID_IDX]);
	pct = attachSharedMemory(pcbTable_Id, 0);
	
	printf("Hello from user\n");	
	return 0;
}

int getSharedMemory() {
	int shmid;

	shmid = shmget(IPC_PRIVATE, getpagesize(), IPC_CREAT | S_IRUSR | S_IWUSR);

	if (shmid == -1) {
		perror("shmget");
		return;
	}
	return shmid;
}
 
void* attachSharedMemory(int shmid, unsigned int readonly) {
	void* paddr;
	int shmflag;

	if (readonly) {
		shmflag = SHM_RDONLY;
	}

	else {
		shmflag = 0;
	}
	paddr = (void*)shmat(shmid, 0, shmflag);

	if (!paddr) {
		perror("shmat");
		return;
	}
	return paddr;
}
