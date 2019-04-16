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
#include <errno.h>                                                              
				                                                                               
#include "shared_memory.h"

int main(int argc, int argv){
	void attachSharedMemory();
	printf("Hello from user\n");	
	return 0;
}


 
