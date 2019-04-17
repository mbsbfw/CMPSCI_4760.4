#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <sys/shm.h> 
#include <sys/wait.h>
#include <sys/time.h>
#include <time.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/queue.h>
#include <sys/stat.h>
#include <errno.h>

#include "shared_memory.h"

#define MSGSZ 50

//SigHand protos 
static void myhandler(int s);
static int setupinterrupt(void);
static int setupitimer(void);


/*
typedef struct{
	unsigned int turnTime;
	Clock waitTime;
	Clock sleepTime;
	Clock idleTime;
	Clock totalCpuTime;
	Clock totalSysTime;
}Stats;
*/

//Global vars
FILE *fp;
 
//Global signal variables
int scheduleId;
int simClockId;
int pcbTableId;
Clock *sysclock;
PCBTable *pct;

struct msgbuf {
	long mtype;
	char mtext[MSGSZ];
}msgbuf;

//Shared memory prototypes
int getSharedMemory();
void *attachSharedMemory(int shmid, unsigned int readonly);
void cleanupSharedMemory(int shmid, void* paddr);
void detachSharedMemory(void* paddr);
void deallocateSharedMemory(int shmid);

//MessageQueue protos
int get_message_queue();
void remove_message_queue(int msgqid);
void receive_msg(int msgqid,struct msgbuf* mbuf, int mtype);
void send_msg(int msgqid,struct msgbuf* mbuf, int mtype);

//Clock prototypes
//struct Clock convertToClockTime(int nanoSec);
void incrementClock(Clock* clock, int increment);
int compareClocks(Clock c1, Clock c2);


int main (int argc, char* argv[]) {

	if(setupinterrupt () == -1){
		perror("Failed to set up handler for SIGPROF");
		return 1;
	}//end if

	if (setupitimer() == -1){
		perror("Failed to set up the ITIMER_PROF interval timer");
		return 1;
	}
	
	int option;
	int maxProcs;

	while((option = getopt(argc, argv, "hn:")) != -1){

		switch(option){

			case 'h' :
				printf("\t\t---Help Menu---\n");
				printf("\t-h Display Help Menu\n");
				printf("\t-n x indicate the maximum total of child processes\n");
				exit(1);
			
			case 'n' :
				maxProcs = atoi(optarg);
				if(maxProcs > 20)
					maxProcs = 18;
				break;

			case '?' :
				printf("ERROR: Improper arguments");
				break;

		}//end switch
	}//end while

	scheduleId = get_message_queue();
	struct msgbuf buf;
	sprintf(buf.mtext, "You've been scheduled!");
	
	unsigned int numOfProcs = 0;
	unsigned int nanoBforeNxtProc = 0;
	Clock forking;
	forking.seconds = 0;
	forking.nanoSec = 0;
	
	//Setup sim clock
	simClockId = getSharedMemory();
	sysclock = (Clock*)(attachSharedMemory(simClockId, 0));
	sysclock->seconds = 0;
	sysclock->nanoSec = 0;

	//setup shared
	pcbTableId = getSharedMemory();
	pct = (PCBTable*)attachSharedMemory(pcbTableId, 0);
	
	nanoBforeNxtProc = rand() % MAX_BETWEEN_PROC;
	//forking = convertToClockTime(nanoBforeNxtProc);
	
	sysclock->seconds = forking.seconds;
	sysclock->nanoSec = forking.nanoSec;

	if(compareClocks(*sysclock, forking) >= 0){
		//create PCB
		PCB pctrlb;
		pctrlb.pid = 1;
		pctrlb.localPid;
		pctrlb.realTime;
		pctrlb.timeQuantum = B_TIME_QUANTUM;
		pctrlb.cpuTimeUsed.seconds = 0, pctrlb.cpuTimeUsed.nanoSec = 0;
		pctrlb.timeInSys.seconds = sysclock->seconds, pctrlb.timeInSys.nanoSec = sysclock->nanoSec;
		pctrlb.timeLastBurst = 0;
		pctrlb.blockedTime.seconds = 0, pctrlb.blockedTime.nanoSec = 0;
		pctrlb.waitTime.seconds = 0, pctrlb.waitTime.nanoSec = 0;
	}

	pid_t pid;
	int ret = 1;
	int status; 
	pid = fork();
	if (pid == -1){
		printf("can't fork, error occured\n");
		exit(EXIT_FAILURE);
	}
	else if (pid == 0){
		printf("child process, pid = %u\n",getpid());


		char * argv_list[] = {"ls","-lart","/home",NULL}; 

		execv("./user",argv_list);
		exit(0);
	}
	else{
		printf("parent process, pid = %u\n",getppid());

		if (waitpid(pid, &status, 0) > 0) {
	  		if (WIFEXITED(status) && !WEXITSTATUS(status))
				printf("program execution successfull\n");

				else if (WIFEXITED(status) && WEXITSTATUS(status)) { 
					if (WEXITSTATUS(status) == 127) { 
						// execv failed 
						printf("execv failed\n");
					}
					else
						printf("program terminated normally,"
						     " but returned a non-zero status\n");
				}
				else{
					// waitpid() failed 
					printf("waitpid() failed\n");
				}
				exit(0);
			}		
	}
	return 0;

}//end main

int getSharedMemory() {
	int shmid;

	shmid = shmget(IPC_PRIVATE, getpagesize(), IPC_CREAT | S_IRUSR | S_IWUSR);

	if (shmid == -1) {
		perror("shmget");
		exit(1);
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
		exit(1);
	}
	return paddr;
}

void cleanSharedMemory(int shmid, void* paddr) {
	detachSharedMemory(paddr);
	deallocateSharedMemory(shmid);
}

void detachSharedMemory(void* paddr) {
	if (shmdt(paddr) == -1) {
		perror("shmdt");
		exit(1);
	}
}

void deallocateSharedMemory(int shmid) {
	if (shmctl(shmid, IPC_RMID, 0) == 1) {
		perror("shmctl");
		exit(1);
	}
}

int get_message_queue() {
	int msgqid;

	msgqid = msgget(IPC_PRIVATE, IPC_CREAT | 0666);

	if (msgqid == -1) {
		perror("msgget");
		exit(1);
	}

	return msgqid;
}

void receive_msg(int msgqid, struct msgbuf* mbuf, int mtype) {
	if (msgrcv(msgqid, mbuf, sizeof(mbuf->mtext), mtype, 0) == -1) {
		perror("msgrcv");
		exit(1);
	}
}

void send_msg(int msgqid, struct msgbuf* mbuf, int mtype) {
	mbuf->mtype = mtype;
	if (msgsnd(msgqid, mbuf, sizeof(mbuf->mtext), IPC_NOWAIT) < 0) {
		perror("msgsnd");
		exit(1);
	}
}

void remove_message_queue(int msgqid) {
	if (msgctl(msgqid, IPC_RMID, NULL) == -1) {
		perror("msgctl");
		exit(1);
	}
}

static void myhandler(int s){

	char aster = '*';
	int errsave;
	errsave = errno;
	//cleanShareMem();
	write(STDERR_FILENO, &aster, 1);
	printf("Terminate after 3 REAL life seconds\n");
	//printf("clock:[%d][%d]", myClock[0], myClock[1]);
	exit(1);
	errno = errsave;
}//end myhandler

static int setupinterrupt(void){

	struct sigaction act;
	act.sa_handler = myhandler;
	act.sa_flags = 0;
	return(sigemptyset(&act.sa_mask) || sigaction(SIGPROF, &act, NULL));
}//end setupinterrupt

static int setupitimer(void){

	struct itimerval value;
	value.it_interval.tv_sec  = 3;
	value.it_interval.tv_usec  = 0;
	value.it_value  = value.it_interval;	
	return (setitimer(ITIMER_PROF, &value, NULL));
}//end setupitimer
/*
struct Clock convertToClockTime(int nanoSec){
	Clock clk;
		clk.seconds = 0;
		clk.nanoSec = 0;

	if (nanoSec >= ONE_BILL) {
		nanoSec -= ONE_BILL;
		clk.seconds = 1;
	}
	clk.nanoSec = nanoSec;
	return clk;
}
*/

int compareClocks(Clock c1, Clock c2) {
	if (c1.seconds > c2.seconds){ 
		return 1;
	}
	if ((c1.seconds == c2.seconds) && (c1.nanoSec > c2.nanoSec)){
		return 1;
	}
	if ((c1.seconds == c2.seconds) && (c1.nanoSec == c2.nanoSec)) {
		return 0;
	}
	return -1;
}



