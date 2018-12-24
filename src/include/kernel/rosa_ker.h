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

#ifndef _ROSA_KER_H_
#define _ROSA_KER_H_

#include "rosa_def.h"
#include "include/kernel/semaphore.h"

/***********************************************************
 * Global variables
 ***********************************************************/
extern tcb * TCBLIST;

extern tcb * SUSPENDEDLIST;

extern tcb * ROUNDROBIN_end;

extern ROSA_taskHandle_t IDLETASK;

extern tcb * EXECTASK;

extern uint64_t system_ticks;

extern uint64_t round_robin_ticks;

//#define MAX_ROUND_ROBIN_TICKS 0
//
//#define ROUND_ROBIN_MODE_ENABLED (MAX_ROUND_ROBIN_TICKS > 1)
/***********************************************************
 * Kernel low level context switch functions
 ***********************************************************/
extern void ROSA_contextInit(tcb * tcbTask);
extern void ROSA_yield(void);


/***********************************************************
 * Other kernel functions
 ***********************************************************/
uint64_t ROSA_getTickCount(void);

uint16_t ROSA_taskCreate( ROSA_taskHandle_t * th, char * id, void * taskFunction, uint32_t stackSize, uint8_t priority );
uint16_t ROSA_taskCreate_critical(ROSA_taskHandle_t* th, char* id, void* taskFunction, uint32_t stackSize, uint8_t priority);
uint16_t taskInstall( tcb * task );
uint16_t insert_after( tcb * position, tcb * new_task );
uint16_t taskUninstall( tcb * task );
uint16_t taskUnsuspend( tcb * task );
uint16_t remove( tcb * removed_task );
uint16_t remove_from_all( tcb * removed_task );
uint16_t ROSA_taskDelete( ROSA_taskHandle_t th );
uint16_t ROSA_taskDelete_noncritical( ROSA_taskHandle_t th );

uint16_t ROSA_delay( uint64_t ticks );
uint16_t ROSA_delayUntil( uint64_t* lastWakeTime, uint64_t ticks );
uint16_t ROSA_delayAbsolute( uint64_t ticks );

void set_round_robin_end(void);

void idle(void);
/***********************************************************
 * General kernel API
 *
 * Comment:
 *	No comments available.
 *
 **********************************************************/

//Define the initial value of the status register
#define ROSA_INITIALSR 0x1c0000

//Timer interrupt service routine
__attribute__((__interrupt__)) extern void timerISR(void);

//Initialize the kernel
void ROSA_init(void);
void ROSA_tcbCreate(tcb * tcbTask, char tcbName[NAMESIZE], void *tcbFunction, int * tcbStack, int tcbStackSize);

//Install a new task TCB into ROSA
extern void ROSA_tcbInstall(tcb *task);

//Start running the ROSA kernel
//This start running the created and installed tasks.
extern void ROSA_start(void);

#endif /* _ROSA_KER_H_ */
