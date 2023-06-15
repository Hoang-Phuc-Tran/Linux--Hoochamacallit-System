/*
 * Filename:	DR.c
 * By:	    	Caine Phung - ID: 6983324
                Hoang Phuc Tran - ID: 8789102
				Philip Wojdyna - ID: 8640180

 * Date:		March 9, 202
 * Description: The data reader (DR) program’s purpose is to monitor its incoming message queue for the varying
statuses of the different Hoochamacallit machines on the shop floor. It will keep track of the number
of different and active machines present in the system.
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
#include "../inc/DR.h"

int main(int argc, char *argv[])
{
    // Declare an array of struct
    MasterList *masterList;

    // Message key for queue message
    key_t message_key;
    int msgID;

    // Get the unique token for the message queue
    message_key = ftok ("../../common/", 'A');
    if (message_key == -1) 
    { 
        return 1;
    }

    // create the message queue
    msgID = msgget (message_key, IPC_CREAT | 0660);
    if (msgID == -1) 
    {

        return 2;
    }

    // shared memory key
    key_t memory_key;
    int memory_ID;

    // Get the unique token for the area of shared memory
    memory_key = ftok(".", 16535); 
    if (memory_key == -1) 
	{ 

	  return 1;
	}

    // Check if shared memory exists, if it's not, we allocate the block of memory
    if ((memory_ID = shmget(memory_key, sizeof(MasterList), 0)) == -1)
   	{
   		// Create shared memory
     	memory_ID = shmget(memory_key, sizeof(MasterList), IPC_CREAT | 0660);
        if (memory_ID == -1) 
		{
		  return 2;
		}
   	}

    // Attach to our shared memory by generating a pointer to the master list
    masterList = (MasterList*)shmat(memory_ID, NULL, 0);	
    if (masterList == NULL) 
	{
	  return 3;
	}

    // Set the values of masterList
	masterList->msgQueueID = msgID;
	masterList->numberOfDCs = 0;
	FILE *logToFile = fopen("../../common/tmp/dataReader.log", "w");

    //the DR will sleep for 15 seconds
    sleep(15);

    // Calculate size of data portion of message 
	int sizeOfData;
    sizeOfData = sizeof (MasterList) - sizeof (long);

    // use to run the loop
    int run = 1;
	int counter = 0;

	// This truct contains the message from DC
	QueueMsg msgReceived;

    while (run)
    {
        // Check to Get the first message in the queue
		if (msgrcv(msgID, &msgReceived, sizeOfData, 0, IPC_NOWAIT) != -1)
		{
			checkMsgFromMachineID(masterList, msgReceived, logToFile);
			
			fflush(stdout);
		}

        //if the value is -1 , something bad happens
		else
		{
			run = 0;
		}

		int counter = 0;

		if (masterList->numberOfDCs != 0)
		{
			run = ((msgID = msgget(message_key, 0)) != -1);
		}

		// Check if the machine is idle more than 35 seconds
		// Loop to check PID to remove it from the masterList, log to file
		for(counter = 0; (counter < MAX_DC_ROLES) && (counter < masterList->numberOfDCs); counter++)
		{
			if (((int)time(NULL) - masterList->dc[counter].lastTimeHeardFrom) > 35)
			{
				// Print information to log
				printTime(logToFile);
				fprintf(logToFile, "DC-%.2d [%d] removed from master list - NON-RESPONSIVE\n\n", counter + 1, masterList->dc[counter].dcProcessID);

				// Shift the array down one index.
				memmove(&masterList->dc[counter], &masterList->dc[counter+1], sizeof(DCInfo) * (MAX_DC_ROLES - (counter + 1)));
				masterList->numberOfDCs--;
			}
		}
		sleep(1.5);
    }

	// If after sleeping 15s, no DCs connects to DR, we terminate the DR, log to file
	printTime(logToFile);
	fprintf(logToFile, "All DCs have gone offline or terminated - DR TERMINATING\n\n");

	// error checking
	if (fclose(logToFile) != 0)
	{
		// can't close it 
		return 4;	
	}	

	// Clear masterList
	shmdt(masterList);

	// Close Shared Memory
	shmctl(memory_ID, IPC_RMID, 0);

	//Close Message Queue 
	msgctl(msgID, IPC_RMID, NULL);

    return 0;
}

/*
* METHOD : 		void checkMsgFromMachineID(MasterList* masterList, QueueMsg msgReceived, FILE *logToFile)
* DESCRIPTION : This function will check the PID of DC machine before log the message to a file. If the PID is known
				we update the time for DC in masterList, if it is not known we add new DC machine to masterList
* PARAMETERS :  MasterList* masterList 
				QueueMsg msgReceived
				FILE *logToFile
* RETURN: NONE
*/
void checkMsgFromMachineID(MasterList* masterList, QueueMsg msgReceived, FILE *logToFile)
{
	// boolean for checking PID from a machine is known or not
	bool isKnown = false;
	// index of current machine from an array masterList
	int index_masterList = 0;

	// Loop to check PID is known in the masterList before we log message to a file
	for(index_masterList = 0; (index_masterList < MAX_DC_ROLES) && (index_masterList < masterList->numberOfDCs) && (isKnown == false) ; index_masterList++)
	{
		// Find the PID
		if(masterList->dc[index_masterList].dcProcessID == msgReceived.pID )
		{
			// Machine has known
			isKnown = true;

			// Update the counter time for machine
			masterList->dc[index_masterList].lastTimeHeardFrom = (int)time(NULL);
			break;
		}
	}

	// If the PID of a machine hasn't known before
	if(isKnown == false && msgReceived.msgValue == 0)
	{
		printTime(logToFile);
		fprintf(logToFile, "DC-%.2d [%d] added to the master list - NEW DC - ", index_masterList + 1, msgReceived.pID);

		// Update the new machine to the array masterList
		masterList->numberOfDCs++;
		masterList->dc[index_masterList].dcProcessID = msgReceived.pID;
		masterList->dc[index_masterList].lastTimeHeardFrom = (int)time(NULL);
	}

	// If PID is known
	else
	{
		// print header message
		printTime(logToFile);
		fprintf(logToFile, "DC-%.2d [%d] ", index_masterList + 1, msgReceived.pID);

		if (msgReceived.msgValue != 6) 
		{ 
			fprintf(logToFile, "updated in the master list - MSG RECEIVED - "); 
		}
	}

	switch (msgReceived.msgValue)
	{
	case 0:
	{
		fprintf(logToFile, "Status 0 (Everything is OKAY)\n\n");
		break;
	}
	case 1:
	{
		fprintf(logToFile, "Status 1 (Hydraulic Pressure Failure)\n\n");
		break;
	}
	case 2:
	{
		fprintf(logToFile, "Status 2 (Safety Button Failure)\n\n");
		break;
	}
	case 3:
	{
		fprintf(logToFile, "Status 3 (No Raw Material in the Process)\n\n");
		break;
	}
	case 4:
	{
		fprintf(logToFile, "Status 4 (Operating Temperatures Out of Range)\n\n");
		break;
	}
	case 5:
	{
		fprintf(logToFile, "Status 5 (Operator Error)\n\n");
		break;
	}
	case 6:
	{
		fprintf(logToFile, "has gone OFFLINE - removing from master-list\n\n");
		
		// Loop to check PID to remove it from the masterList
		for(index_masterList = 0; (index_masterList < MAX_DC_ROLES) && (index_masterList < masterList->numberOfDCs); index_masterList++)
		{
			// Find the PID
			if(masterList->dc[index_masterList].dcProcessID == msgReceived.pID )
			{
				// Shift the array down one index.
				memmove(&masterList->dc[index_masterList], &masterList->dc[index_masterList + 1], sizeof(DCInfo) * (MAX_DC_ROLES - (index_masterList + 1)));
				break;
			}
		}

		// Decrease a machine from the masterLisst
		masterList->numberOfDCs--;
		break;
	}
	default:
		break;
	}

}



