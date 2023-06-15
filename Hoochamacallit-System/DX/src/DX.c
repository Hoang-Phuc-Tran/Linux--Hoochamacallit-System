/*
 * Filename:	DX.c
 * By:	    	Caine Phung - ID: 6983324
                Hoang Phuc Tran - ID: 8789102
				Philip Wojdyna - ID: 8640180

 * Date:		March 9, 202
 * Description: Data Corrupter.
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
#include "../inc/DX.h"


int main (void)
{	
	char logPath[50] = "../../common/tmp/dataCorruptor.log";
	FILE* fp;
   	fp = fopen (logPath, "a");

	if (fp == NULL)
	{
		return -1;
	}

	int randNum;
	int sizeofdata;
	int shmid;
	int count = 0;
	int mid;
	
	//initialize the random seeds
	srand ((unsigned)time(NULL));  

	MasterList *p;
	key_t shmKey = ftok (".", 16535);

	if (shmKey == -1)
	{
		return 1;
	}

	// get share memory id
	while ((shmid = shmget (shmKey, sizeof (MasterList), 0)) == -1) 
	{
		
        sleep (10);
        count ++;
		if (count == 100)
		{
		  	 return 2;
		}
	}

    // Attach to our shared memory by generating a pointer to the master list
    p = (MasterList *)shmat (shmid, NULL, 0);

	if (p == NULL) 
	{
	  return 3;
	}

    while(true){

		// sleep for a random amount of time from 10 to 30s
		int sleepNum = rand() % (30 - 10 + 1) + 10;
 		sleep (sleepNum); 
		
		// Message key
        key_t message_key;

		// Message ID
	    int mid; 

		// Get message key
    	message_key = ftok ("../../common/", 'A');

	    if (message_key == -1)
	    {    
		    return 0;
	    }

	    if ((mid = msgget (message_key, 0)) != -1) 
	    {
			// generate a random number from 0 to 20
			int num = rand() % 21;
			
			if (num == 10 || num == 17)
			{
				// if message queue already be deleted, log to file
				if ((msgctl(mid, IPC_RMID, NULL) == -1))
				{
					printTime(fp);
					fprintf(fp, "DX Failed to delete the msgQ, must already be deleted - exiting\n\n");
					break;
				}

				// Delete message queue ,log to file
				else 
				{
					printTime(fp);					
					fprintf(fp, "DX Deleted the msgQ - the DR/DCs can't talk anymore - exiting\n\n");
					break;
				}
			}
			//Call the function to kill processes
			wheelOfDestruction(num, p, fp);		
		}
	}

	// error checking
	if (fclose(fp) != 0)
	{
		// can't close it 
		return 4;	
	}	
}



/*
* METHOD : 		void wheelOfDestruction(int randNum, MasterList *masterList,FILE* logFile)
* DESCRIPTION : The random number passed is what determiens which process of DC to kill.
* PARAMETERS :  (int randNum, MasterList *masterList,FILE* logFile)
*/
void wheelOfDestruction(int randNum, MasterList *masterList,FILE* logFile)
{

	int killIndex = 0;
	pid_t killPID = 0;

	// switch case
	switch (randNum)
	{
	case 1:
	case 4:
	case 11:
	{
		killIndex = 1;
		break;
	}
	case 2:
	case 5:
	case 15:
	{
		killIndex = 3;
		break;
	}
	case 3:
	case 6:
	case 13:
	{
		killIndex = 2;
		break;
	}
	case 7:
	{
		killIndex = 4;
		break;
	}
	case 9:
	{
		killIndex = 5;
		break;
	}
	case 12:
	{
		killIndex = 6;
		break;
	}
	case 14:
	{
		killIndex = 7;
		break;
	}
	case 16:
	{
		killIndex = 8;
		break;
	}
	case 18:
	{
		killIndex = 9;
		break;
	}
	case 20:
	{
		killIndex = 10;
		break;
	}
	}
	// Check the conditon the index is killed less than and euqal number DCs in list
	if (killIndex <= masterList->numberOfDCs) 
	{
		//Get the PID from the shared memory 
		killPID = masterList->dc[killIndex - 1].dcProcessID; 

		// killing process succeed
		if (!kill(killPID, SIGHUP))
		{
			printTime(logFile);
			fprintf(logFile, "WOD Action %.2d - D-%.2d [%d] TERMINATED\n\n", randNum, killIndex, (int)killPID);
		}
	}
}



