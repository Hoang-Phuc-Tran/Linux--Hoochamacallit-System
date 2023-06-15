/*
 * Filename:	DC.c
 * By:	    	Caine Phung - ID: 6983324
                Hoang Phuc Tran - ID: 8789102
				Philip Wojdyna - ID: 8640180

 * Date:		March 9, 202
 * Description: The data creator (DC) program’s job is to generate a status condition representing the
state of the Hoochamacallit machine.
 */
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <time.h>
#include <sys/shm.h>
#include <stdbool.h>
#include "../../common/inc/common.h"
#include "../inc/DC.h"

int main (int argc, char *argv[])
{
	QueueMsg m;
	int count = 0;
	int rc;	// return code from message processing

	srand ((unsigned)time(NULL));   //initialize the random seeds
	
	key_t message_key;
	int mid = 0;// message ID
	message_key = ftok ("../../common/", 'A');

	//get message key
	if (message_key == -1)
	{
		return 0;
	}
	
	//get queue id
	while ((mid = msgget (message_key, 0)) == -1) {
		sleep (10);
	}	

	//get pid
	m.pID = getpid();
	m.msgValue = 0;

	//send the first message and log to file
	rc = msgsnd (mid, (void *)&m, sizeof (QueueMsg)- sizeof(long), 0);
	
	if (rc == -1) 
	{
		return 4;
	}
	logDC("../../common/tmp/dataCreator.log", m.pID, "Status 0 (Everything is OK)");

	bool offline = false;

	while (mid != -1)
	{
		// generate a random number 
 		int number = (int)(rand() % 6) + 1;
		m.msgValue = number;

		//sending message
		rc = msgsnd (mid, (void *)&m, sizeof (QueueMsg)- sizeof(long), 0);

		if (rc == -1) 
		{
			return 4;
		}
		
		char str[50];
	
		// switch case
		switch(m.msgValue)
		{
		case 0:
			strcpy(str, "Status 0 (Everything is OK)");
			break;	

		case 1:
			strcpy(str, "Status 1 (Hydraulic Pressure Failure)");
			break;			

		case 2:
			strcpy(str, "Status 2 (Safety Button Failure)");
			break;			

		case 3:
			strcpy(str, "Status 3 (No Raw Material in the Process)");
			break;			

		case 4:				
			strcpy(str, "Status 4 (Operating Temperature Out of Range)");
			break;		

		case 5:
			strcpy(str, "Status 5 (Operator Error)");
			break;

		case 6:
			mid = -1;
			strcpy(str, "Status 6 (Machine is Off-line)");
			break;

		default:
			break;
		}	
	
		//log to file
		logDC("../../common/tmp/dataCreator.log", m.pID, str);

		// sleep at random amount from 10 to 30s
		int sleepNum = rand() % (30 - 10 + 1) + 10;
 		sleep (sleepNum);  
	}

	return 0;
}


/*
* METHOD : 		int logDC(char * path, int pid,char* message)
* DESCRIPTION : Log time and messages to file.
* PARAMETERS :  (char * path, int pid,char* message)
* NONE: 0 indicates successful, otherwise negative numbers
*/
int logDC(char * path, int pid,char* message){

		FILE * fp;
   		fp = fopen (path, "a");
		if (fp == NULL)
		{
			return -1;
		}

		// Get the localtime		
    	struct tm* local;
    	time_t t = time(NULL);
  
		// Get the localtime
    	local = localtime(&t);
		fprintf(fp, "%sDC [%d] – MSG SENT – %s\n\n",asctime(local),pid, message);
   		fflush(fp);

		// error checking
   		if (fclose(fp) != 0)
		{
		// can't close it 
		return -2;	
		}	

		return 0;
}











