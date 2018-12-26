/*****************************************************************************

                 ,//////,   ,////    ,///' /////,
                ///' ./// ///'///  ///,    ,, //
               ///////,  ///,///   '/// //;''//,
             ,///' '///,'/////',/////'  /////'/;,

    Copyright 2010 Marcus Jansson <mjansson256@yahoo.se>

    This file is part of ROSA - Realtime Operating System for AVR32.

    ROSA is free software: you can redistribute it and/or modify
    it under the terms of the GNU General Public License as published by
    the Free Software Foundation, either version 3 of the License, or
    (at your option) any later version.

    ROSA is distributed in the hope that it will be useful,
    but WITHOUT ANY WARRANTY; without even the implied warranty of
    MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
    GNU General Public License for more details.

    You should have received a copy of the GNU General Public License
    along with ROSA.  If not, see <http://www.gnu.org/licenses/>.
*****************************************************************************/
/* Tab size: 4 */

#ifndef rosa_def_H_
#define rosa_def_H_

#include "rosa_config.h"
#include "stdint.h"

#ifndef NULL
#define NULL 0
#endif

// If SEMAPHORE_PROTECTIONS is non-zero, tasks deleted using ROSA_taskDelete will be forced to unlock any semaphore that they may hold.
// Deleted tasks will also be safely removed from any semaphore BLOCKEDLISTs that they be in.
#define SEMAPHORE_PROTECTIONS 1

// If ROUND_ROBIN_MODE_ENABLED is non-zero, all simultaneously-ready, highest-priority tasks will be run with round robing scheduling.
// If only one highest-priority task is ready at any point in execution, round robin will not be used.
#define ROUND_ROBIN_MODE_ENABLED 0

// MAX_ROUND_ROBIN_TICKS is the number of system ticks that each task in the round robin execution is granted.
#define MAX_ROUND_ROBIN_TICKS 10

// If IDLE_TASK_ENABLED is non-zero, an idle task will be created and executed when no other tasks are ready for execution.
#define IDLE_TASK_ENABLED 1

/***********************************************************
 * TCB block
 *
 * Comment:
 * 	This struct contain all the necessary information to
 * 	do a context switch
 *
 **********************************************************/

typedef struct semaphore_t;

typedef struct tcb_record_t
{
	struct tcb_record_t * nexttcb;
	char id[NAMESIZE];		//The task id/name
	void (*staddr) (void);	//Start address
	int *dataarea;			//The stack data area
	int datasize;			//The stack size
	int *saveusp;			//The current stack position
	int SAVER0;				//Temporary work register
	int SAVER1;				//Temporary work register
	int savesr;				//The current status register
	int retaddr;			//The return address
	int savereg[15];		//The CPU registers
	
	struct tcb_record_t * prevtcb;				// a pointer to the previous tcb in whatever list the tcb is in (used to implement doubly linked list data structure)
	uint8_t priority;							// the standard priority of a task that is given by the user at task creation
	uint8_t effective_priority;					// the priority of a task that is used by the scheduler (affected by the semaphores that the task holds)
	uint64_t back_online_time;					// the time at which a suspended task is set to resume execution
	struct semaphore_t * blocking_semaphore;	// a task can only be blocked by a single semaphore at any given time
} tcb;

#define ROSA_taskHandle_t tcb*

#endif /* rosa_def_H_ */
