#include <stdlib.h>
#include <unistd.h>
#include <stdio.h>
#include <sys/shm.h>
#include <sys/stat.h>

#include "shared_memory.h"

int sysClockShmid;

void attachSharedMemory(){

	sysClockShmid = shmget(CLOCK_KEY, sizeof(AllS), IPC_CREAT|0777);

	if(sysClockShmid == -1){
		perror("shmget");
		exit(1);
	}

	void *paddr;
	int shmflag;

	if (readonly) {
		shmflag = SHM_RDONLY;
	}
	else {
		shmflag = 0;
	}
	paddr = (void*)shmat(sysClockShmid, 0, shmflag);

	if (!paddr){
		perror("shmat");
		exit(1);
	}
}

