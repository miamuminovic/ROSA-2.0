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

tcb * iterator;

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
	
// for handling possible round robin configuration
/*******************************************************************/
#if ROUND_ROBIN_MODE_ENABLED
	//if( ROUNDROBIN_end )
	//{
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
	//}
#endif
/*******************************************************************/
	
	while( SUSPENDEDLIST && SUSPENDEDLIST->back_online_time <= system_ticks )
	{
		iterator = SUSPENDEDLIST;
		taskUnsuspend(iterator);
		taskInstall(iterator);
	}
	
	context_switch_time = *(uint32_t*)(0xFFFF3850);
	
	EXECTASK = TCBLIST;

	context_switch_time = *(uint32_t*)(0xFFFF3850) - context_switch_time;
	if(context_switch_time < 10000)
	{
		total_context_switch_time += context_switch_time;
	}

}
