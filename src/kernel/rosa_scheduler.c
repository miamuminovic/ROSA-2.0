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

#include "include/kernel/rosa_tim.h"
#include "kernel/rosa_scheduler.h"

/***********************************************************
 * scheduler
 *
 * Comment:
 * 	Minimalistic scheduler for round robin task switch.
 * 	This scheduler choose the next task to execute by looking
 * 	at the nexttcb of the current running task.
 **********************************************************/
void scheduler(void)
{

	if(system_ticks == 2407)
	{
 		int i = 0;
	}

// for handling possible round robin configuration
/*******************************************************************/
#if ROUND_ROBIN_MODE_ENABLED
	if( ROUNDROBIN_end )
	{
		round_robin_ticks ++;
		
		if( round_robin_ticks >= MAX_ROUND_ROBIN_TICKS )
		{
			// this really should be possible to optimize
			taskUninstall(EXECTASK);
			taskInstall(EXECTASK);
			
			// this code is almost functional, but not quite. low priority tasks run even in the presence of non-delayed, high priority tasks
			//TCBLIST = TCBLIST->nexttcb;
			//remove(EXECTASK);
			//insert_after(ROUNDROBIN_end, EXECTASK);
			//ROUNDROBIN_end = EXECTASK;
			
			round_robin_ticks = 0;
		}
	}
#endif
/*******************************************************************/

	tcb * iterator = SUSPENDEDLIST;
	while( iterator && iterator->back_online_time <= system_ticks )
	{
		taskUnsuspend(iterator);
		taskInstall(iterator);
		iterator = SUSPENDEDLIST;
	}
	
	// if no tasks are ready, run the idle task
	if( TCBLIST == NULL )
	{
		EXECTASK = IDLETASK;
	}
	// if at least one task is ready, run the highest priority task
	else
	{
		EXECTASK = TCBLIST;
	}
}
