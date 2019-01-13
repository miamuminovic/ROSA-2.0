/*****************************************************************************

                 ///////,   .////    .///' /////,
                ///' ./// ///'///  ///,     '///
               ///////'  ///,///   '/// //;';//,
             ,///' ////,'/////',/////'  /////'/;,

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

//Kernel includes
#include "kernel/rosa_def.h"
#include "kernel/rosa_ext.h"
#include "kernel/rosa_ker.h"
#include "kernel/rosa_tim.h"
#include "kernel/rosa_scheduler.h"

//Driver includes
#include "drivers/button.h"
#include "drivers/led.h"
#include "drivers/pot.h"
#include "drivers/usart.h"

/***********************************************************
 * TCBLIST
 *
 * Comment:
 * 	Global variables that contain the list of TCB's that
 * 	have been installed into the kernel with ROSA_tcbInstall()
 **********************************************************/
tcb * TCBLIST;

tcb * SUSPENDEDLIST;

//tcb * ROUNDROBIN_end;

#if IDLE_TASK_ENABLED
ROSA_taskHandle_t IDLETASK;
#endif

semaphore * SEMAPHORES;

/***********************************************************
 * EXECTASK
 *
 * Comment:
 * 	Global variables that contain the current running TCB.
 **********************************************************/
uint64_t system_ticks;

uint64_t round_robin_ticks;

tcb * EXECTASK;

/***********************************************************
 * ROSA_init
 *
 * Comment:
 * 	Initialize the ROSA system
 *
 **********************************************************/
void ROSA_init(void)
{
	//Do initialization of I/O drivers
	ledInit();									//LEDs
	buttonInit();								//Buttons
	joystickInit();								//Joystick
	potInit();									//Potentiometer
	usartInit(USART, &usart_options, FOSC0);	//Serial communication

	//Start with empty TCBLIST and no EXECTASK.
	TCBLIST				= NULL;
	SUSPENDEDLIST		= NULL;
	EXECTASK			= NULL;
	SEMAPHORES			= NULL;
	
#if IDLE_TASK_ENABLED
	IDLETASK = NULL;
	ROSA_taskCreate(&IDLETASK, "idle", idle, 0x40, 255);
	//taskUninstall(IDLETASK);
#endif
	
	//initialize system time
	system_ticks = 0;
	round_robin_ticks = 0;
	
	//Initialize the timer to 1 ms period.
	//...
	interruptInit();
	timerInit(1);
	//...
}

uint64_t ROSA_getTickCount(void)
{
	return system_ticks;
}

uint16_t insert_after( tcb * position, tcb * new_task )
{
	uint16_t result = -1;
	
	if( position != NULL )
	{
		new_task->prevtcb = position;
		new_task->nexttcb = position->nexttcb;
		
		position->nexttcb = new_task;
		if( new_task->nexttcb != NULL )
		{
			(new_task->nexttcb)->prevtcb = new_task;
		}
		result = 1;
	}
	return result;
}

/***********************************************************
 * ROSA_tcbCreate
 *
 * Comment:
 * 	Create the TCB with correct values.
 *
 **********************************************************/
void ROSA_tcbCreate(tcb * tcbTask, char tcbName[NAMESIZE], void *tcbFunction, int * tcbStack, int tcbStackSize)
{
	int i;

	//Initialize the tcb with the correct values
	for(i = 0; i < NAMESIZE; i++) {
		//Copy the id/name
		tcbTask->id[i] = tcbName[i];
	}

	//Dont link this TCB anywhere yet.
	tcbTask->nexttcb = NULL;

	//Set the task function start and return address.
	tcbTask->staddr = tcbFunction;
	tcbTask->retaddr = (int)tcbFunction;

	//Set up the stack.
	tcbTask->datasize = tcbStackSize;
	tcbTask->dataarea = tcbStack + tcbStackSize;
	tcbTask->saveusp = tcbTask->dataarea;

	//Set the initial SR.
	tcbTask->savesr = ROSA_INITIALSR;

	//Initialize context.
	contextInit(tcbTask);
}


/***********************************************************
 * ROSA_tcbInstall
 *
 * Comment:
 * 	Install the TCB into the TCBLIST.
 *
 **********************************************************/
void ROSA_tcbInstall(tcb * tcbTask)
{
	tcb * tcbTmp;

	/* Is this the first tcb installed? */
	if(TCBLIST == NULL) {
		TCBLIST = tcbTask;
		TCBLIST->nexttcb = tcbTask;			//Install the first tcb
		tcbTask->nexttcb = TCBLIST;			//Make the list circular
	}
	else {
		tcbTmp = TCBLIST;					//Find last tcb in the list
		while(tcbTmp->nexttcb != TCBLIST) {
			tcbTmp = tcbTmp->nexttcb;
		}
		tcbTmp->nexttcb = tcbTask;			//Install tcb last in the list
		tcbTask->nexttcb = TCBLIST;			//Make the list circular
	}
}

uint16_t taskInstall(tcb * task)
{
	uint16_t result = -1;
	tcb * iterator;
	
	//if there are no tasks in the list
	if( TCBLIST == NULL )
	{
		TCBLIST		  = task;
		task->nexttcb = task;
		task->prevtcb = task;

		result = 1;
	}
	//if this task should be inserted before the first task in the list
	else if( task->effective_priority < TCBLIST->effective_priority )
	{
		insert_after(TCBLIST->prevtcb, task);
		TCBLIST = task;
		
		result = 1;
	}
	else if( task->effective_priority >= (TCBLIST->prevtcb)->effective_priority )
	{
		insert_after(TCBLIST->prevtcb, task);
	}
	else
	{
		iterator = TCBLIST;
		while( iterator && task->effective_priority >= iterator->effective_priority )
		{
			iterator = iterator->nexttcb;
		}
		insert_after(iterator->prevtcb, task);
		
		result = 1;
	}

	return result;
}

uint16_t ROSA_taskCreate_critical(ROSA_taskHandle_t* th, char* id, void* taskFunction, uint32_t stackSize, uint8_t priority)
{
	interruptDisable();
	uint16_t result = ROSA_taskCreate(th, id, taskFunction, stackSize, priority);
	interruptEnable();
	
	return result;
}

uint16_t ROSA_taskCreate(ROSA_taskHandle_t* th, char* id, void* taskFunction, uint32_t stackSize, uint8_t priority)
{
	uint16_t result = 0;
	
	if(! *th)
	{
		*th = (tcb*)calloc(1, sizeof(tcb));	//allocate memory for th
		tcb* temp = *th;
		
		int i;
		for(i = 0; i < NAMESIZE; i ++)
		{
			temp->id[i] = id[i];		//copy task id
		}
		
		temp->staddr = taskFunction;
		temp->retaddr = (int)taskFunction;
		
		temp->datasize = stackSize;
		temp->dataarea = (int*)calloc(stackSize, sizeof(int)) + stackSize;
		temp->saveusp = temp->dataarea;
		
		temp->savesr = ROSA_INITIALSR;
		
		temp->priority = priority;
		temp->effective_priority = priority;
		
		temp->suspended = 0;
		
		contextInit(temp);
		
		taskInstall(* th);
	}
	return result;
}

uint16_t taskSuspend(tcb * task)
{
	uint16_t result = -1;
	tcb * iterator;
	
	//if there are no tasks in the list
	if( SUSPENDEDLIST == NULL )
	{
		SUSPENDEDLIST		= task;
		task->nexttcb		= task;
		task->prevtcb		= task;
		
		result = 1;
	}
	//if this task should be inserted before the first task in the list
	else if( task->back_online_time < SUSPENDEDLIST->back_online_time )
	{
		insert_after(SUSPENDEDLIST->prevtcb, task);
		SUSPENDEDLIST = task;
		
		result = 1;
	}
	//if this task should be inserted after the last task in the list
	else if( task->back_online_time >= (SUSPENDEDLIST->prevtcb)->back_online_time )
	{
		insert_after(SUSPENDEDLIST->prevtcb, task);
		SUSPENDEDLIST->prevtcb = task;
		
		result = 1;
	}
	//if this task should be inserted somewhere in the middle of the list
	else
	{
		iterator = SUSPENDEDLIST;
		while( iterator && task->back_online_time >= iterator->back_online_time )
		{
			iterator = iterator->nexttcb;
		}
		insert_after(iterator->prevtcb, task);
		
		result = 1;
	}
	
	task->suspended = 1;
	
	return result;
}

uint16_t taskUninstall( tcb * task )
{
	uint16_t result = -1;
	
	// change head and tail of TCBLIST if necessary
	if( TCBLIST == task && (TCBLIST->prevtcb) == task )
	{
		TCBLIST	= NULL;
	}
	else if( TCBLIST == task )
	{
		TCBLIST = TCBLIST->nexttcb;
	}
	
	result = remove(task);
	
	result = 1;
	
	return result;
}

uint16_t taskUnsuspend( tcb * task )
{
	uint16_t result = -1;
	
	// change head and tail of SUSPENDEDLIST if necessary
	if( SUSPENDEDLIST == task && SUSPENDEDLIST->prevtcb == task)
	{
		SUSPENDEDLIST		= NULL;
	}
	else if( SUSPENDEDLIST == task )
	{
		SUSPENDEDLIST = SUSPENDEDLIST->nexttcb;
	}
	else if( SUSPENDEDLIST->prevtcb == task )
	{
		SUSPENDEDLIST->prevtcb = (SUSPENDEDLIST->prevtcb)->prevtcb;
	}
	
	result = remove(task);
	task->suspended = 0;
	
	return result;
}

uint16_t remove(tcb * removed_task)
{
	uint16_t result = -1;
	
	if( removed_task->prevtcb != NULL )
	{
		(removed_task->prevtcb)->nexttcb = removed_task->nexttcb;
	}
	
	if( removed_task->nexttcb != NULL )
	{
		(removed_task->nexttcb)->prevtcb = removed_task->prevtcb;
	}
	
	// nullify pointers
	removed_task->prevtcb = NULL;
	removed_task->nexttcb = NULL;
		
	result = 1;
	
	return result;
}

uint16_t ROSA_delay( uint64_t ticks )
{
	interruptDisable();
	uint16_t result = -1;
	
	taskUninstall(EXECTASK);
	EXECTASK->back_online_time = ROSA_getTickCount() + ticks;
	taskSuspend(EXECTASK);
	
	result = ticks;
	
	ROSA_yield();
	interruptEnable();
	return result;
}

uint16_t ROSA_delayUntil( uint64_t* lastWakeTime, uint64_t ticks )
{
	interruptDisable();
	uint16_t result = -1;
	
	taskUninstall(EXECTASK);
	EXECTASK->back_online_time = (* lastWakeTime) + ticks;
	(* lastWakeTime) = (* lastWakeTime) + ticks;
	taskSuspend(EXECTASK);
	
	result = ticks - (* lastWakeTime);
	
	ROSA_yield();
	interruptEnable();
	return result;
}

uint16_t ROSA_delayAbsolute( uint64_t ticks )
{
	interruptDisable();
	uint16_t result = -1;
	taskUninstall(EXECTASK);
	EXECTASK->back_online_time = ticks;
	taskSuspend(EXECTASK);
	ROSA_yield();
	interruptEnable();
	return result;
}

#if IDLE_TASK_ENABLED
void idle(void)
{
	while(1);
}
#endif

uint16_t ROSA_taskDelete(ROSA_taskHandle_t th)
{
	interruptDisable();
	
	uint16_t result = ROSA_taskDelete_noncritical(th);
	
	// if we have just deleted EXECTASK
	if( result == 10 )
	{
		ROSA_yield();
	}
	interruptEnable();
	
	return result;
}

uint16_t ROSA_taskDelete_noncritical(ROSA_taskHandle_t th)
{
	int deleting_active_task = 0;
	if( th == NULL )
	{
		th = EXECTASK;
		deleting_active_task = 1;
	}
	
	uint16_t result = -1;

// semaphore protections
/**********************************************************************/
#if SEMAPHORE_PROTECTIONS
	semaphore * iterator = SEMAPHORES;
	while( iterator != NULL )
	{
		if( iterator->current_task == th )
		{
			semaphoreEvictCurrentTask( iterator );
		}
	
		if( iterator->nextsemaphore == SEMAPHORES )
		{
			iterator = NULL;
		}
		else
		{
			iterator = iterator->nextsemaphore;
		}
	}

	if( th->blocking_semaphore != NULL )
	{
		semaphoreUnblockTask(th->blocking_semaphore, th);
	}
#endif
/**********************************************************************/
	
	// Tasks can be deleted both from the TCBLIST and from the SUSPENDEDLIST
	//result = taskUninstall(th);
	result = remove_from_all(th);
	
	if(result == 1)
	{
		free(th->dataarea - th->datasize);
		memset(th, 0, sizeof(tcb));
		free(th);
		
		if( deleting_active_task )
		{
			result = 10;
		}
	}
	
	return result;
}

uint16_t remove_from_all( tcb * removed_task )
{
	uint16_t result = -1;
	
	// change head and tail of TCBLIST if necessary
	if( TCBLIST == removed_task && (TCBLIST->prevtcb) == removed_task )
	{
		TCBLIST	= NULL;
	}
	else if( TCBLIST == removed_task )
	{
		TCBLIST = TCBLIST->nexttcb;
	}
	// change head and tail of SUSPENDEDLIST if necessary
	else if( SUSPENDEDLIST == removed_task && SUSPENDEDLIST->prevtcb == removed_task)
	{
		SUSPENDEDLIST		= NULL;
	}
	else if( SUSPENDEDLIST == removed_task )
	{
		SUSPENDEDLIST = SUSPENDEDLIST->nexttcb;
	}
	else if( SUSPENDEDLIST->prevtcb == removed_task )
	{
		SUSPENDEDLIST->prevtcb = (SUSPENDEDLIST->prevtcb)->prevtcb;
	}

	if( removed_task->prevtcb != NULL )
	{
		(removed_task->prevtcb)->nexttcb = removed_task->nexttcb;
	}
	
	if( removed_task->nexttcb != NULL )
	{
		(removed_task->nexttcb)->prevtcb = removed_task->prevtcb;
	}
	
	// nullify pointers
	removed_task->prevtcb = NULL;
	removed_task->nexttcb = NULL;
		
	result = 1;
	
	return result;
}