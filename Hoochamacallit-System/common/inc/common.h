/*
 * Filename:	common.h
 * By:	    	Caine Phung - ID: 6983324
                Hoang Phuc Tran - ID: 8789102
				Philip Wojdyna - ID: 8640180

 * Date:		March 9, 202
 * Description: Common file to be used for DC, DR, and DX.
 */
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/shm.h>
#include <unistd.h>
#include <stdbool.h>
#include <time.h>
#include <signal.h>



// Maximum number of DCs running at one time
#define MAX_DC_ROLES			10

// prototype
int printTime(FILE* logFile);



/*
* METHOD : 		int printTime(FILE* logFile)
* DESCRIPTION : Log time to file.
* PARAMETERS :  (FILE* logFile)
*/
int printTime(FILE* logFile){

        // Get the localtime
        struct tm* local;
        time_t t = time(NULL);

        // Get the localtime
        local = localtime(&t);
        fprintf(logFile, "%s", asctime(local));
        fflush(logFile);
        return 0;
}

typedef struct tagQueueMsg
{
	// UNIX/Linux requirement
	long type;
	// The contents of the message
	int msgValue;
	pid_t pID;
} QueueMsg;

typedef struct tagDCInfo
{
	pid_t dcProcessID;
	int lastTimeHeardFrom;
} DCInfo;

typedef struct tagMasterList
{
	int msgQueueID;
	int numberOfDCs;
	DCInfo dc[MAX_DC_ROLES];
} MasterList;
