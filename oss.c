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
#include <errno.h>

#include "shared_memory.h"

int get_message_queue();
void remove_message_queue(int msgqid);
void receive_msg(int msgqid, struct msgbuf* mbuf, int mtype);
void send_msg(int msgqid, struct msgbuf* mbuf, int mtype);


struct Stats{
	unsigned int turnTime;
	Clock waitTime;
	Clock sleepTime;
	Clock idleTime;
	Clock totalCpuTime;
	Clock totalSysTime;
};

struct Stats resetStats();
processCtrlTable *aGo;
 
//Global signal variables
int scheduleId;

int main (int argc, char* argv[]) {
		  
	void attachSharedMemory();
	scheduleId = get_message_queue();
	struct msgbuf buf;
	sprintf(buf.mtext, "You've been scheduled!");
	
	pid_t  pid;
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

struct Stats resetStats(){
	struct Stats stats = {
		.turnTime = 0,
		.waitTime = {.seconds = 0, .nanoSec = 0},
		.sleepTime = {.seconds = 0, .nanoSec = 0},
		.idleTime = {.seconds = 0, .nanoSec = 0},
		.totalCpuTime = {.seconds = 0, .nanoSec = 0},
		.totalSysTime = {.seconds = 0, .nanoSec = 0},
	};
	return stats;
}

int get_message_queue() {
	int msgqid;

	msgqid = msgget(MSG_QUEUE, IPC_CREAT | 0666);

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





