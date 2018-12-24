#include "include/kernel/semaphore.h"

int16_t semaphore_insert_after(semaphore * position, semaphore * new_semaphore)
{
	new_semaphore->nextsemaphore = position->nextsemaphore;
	new_semaphore->prevsemaphore = position;
	
	position->nextsemaphore = new_semaphore;
	new_semaphore->nextsemaphore->prevsemaphore = new_semaphore;
}

int16_t semaphoreAdd(semaphore * handle)
{
	int16_t result = -1;
	
	if( SEMAPHORES == NULL )
	{
		SEMAPHORES = handle;
		SEMAPHORES->nextsemaphore = SEMAPHORES;
		SEMAPHORES->prevsemaphore = SEMAPHORES;
		
		result = 1;
	}
	else
	{
		semaphore_insert_after(SEMAPHORES->prevsemaphore, handle);
		
		result = 1;
	}
	
	return result;
}

int16_t ROSA_semaphoreCreate(ROSA_semaphoreHandle_t * handle, uint8_t ceiling)
{
	interruptDisable();
	int16_t result = -1;
	(*handle) = calloc(1, sizeof(semaphore));
	(*handle)->ceiling = ceiling;
	result = abs(*handle);
	semaphoreAdd(*handle);
	interruptEnable();
	
	return result;
}

int16_t semaphoreSetCurrentTask( semaphore * semaphore_handle, tcb * task_handle )
{
	int16_t result = 0;
	
	semaphore_handle->current_task = task_handle;
	semaphore_handle->task_priority = task_handle->effective_priority;
	if( task_handle->effective_priority > semaphore_handle->ceiling )
	{
		task_handle->effective_priority = semaphore_handle->ceiling;
	}
	
	return result;
}

int16_t semaphoreUnsetCurrentTask( semaphore * semaphore_handle )
{
	int16_t result = 0;
	
	semaphore_handle->current_task->effective_priority = semaphore_handle->task_priority;
	semaphore_handle->current_task = NULL;
	
	return result;
}

int16_t semaphoreBlockTask( semaphore * semaphore_handle, tcb * new_task )
{
	int16_t result = -1;
	
	new_task->blocking_semaphore = semaphore_handle;
	
	if( semaphore_handle->BLOCKEDLIST == NULL )
	{
		semaphore_handle->BLOCKEDLIST = new_task;
		semaphore_handle->BLOCKEDLIST->nexttcb = new_task;
		semaphore_handle->BLOCKEDLIST->prevtcb = new_task;
		
		result = 1;
	}
	else
	{
		insert_after(semaphore_handle->BLOCKEDLIST->prevtcb, new_task);
		
		result = 1;
	}
	
	return result;
}

int16_t semaphoreUnblockTask( semaphore * semaphore_handle, tcb * task_handle )
{
	int16_t result = 0;
	
	//tcb * temp = semaphore_handle->BLOCKEDLIST;
	//(*task_handle) = semaphore_handle->BLOCKEDLIST;
	
	// only one blocked task
	if( semaphore_handle->BLOCKEDLIST == semaphore_handle->BLOCKEDLIST->prevtcb )
	{
		semaphore_handle->BLOCKEDLIST = NULL;
	}
	// more than one blocked task
	else if( task_handle == semaphore_handle->BLOCKEDLIST )
	{
		semaphore_handle->BLOCKEDLIST = semaphore_handle->BLOCKEDLIST->nexttcb;
	}
	
	remove( task_handle );
	
	task_handle->blocking_semaphore = NULL;
	
	return result;
}

int16_t semaphore_remove(semaphore * removed_semaphore)
{
	if( removed_semaphore == SEMAPHORES )
	{
		SEMAPHORES = SEMAPHORES->nextsemaphore;
	}
	
	removed_semaphore->nextsemaphore->prevsemaphore = removed_semaphore->prevsemaphore;
	removed_semaphore->prevsemaphore->nextsemaphore = removed_semaphore->nextsemaphore;
	
	removed_semaphore->nextsemaphore = NULL;
	removed_semaphore->prevsemaphore = NULL;
}

int16_t ROSA_semaphoreDelete(ROSA_semaphoreHandle_t handle)
{
	interruptDisable();
	
	int16_t result = -1;
	if( handle->current_task == NULL && handle->BLOCKEDLIST == NULL )
	{
		semaphore_remove(handle);
		free(handle);
		result = 1;
	}
	
	interruptEnable();
	
	return result;
}

int16_t ROSA_semaphorePeek(ROSA_semaphoreHandle_t handle)
{
	return (handle->current_task != NULL);
}

int16_t ROSA_semaphoreLock(ROSA_semaphoreHandle_t handle)
{
	interruptDisable();
	int16_t result = 0;
	if( handle->current_task == NULL )
	{
		// If the semaphore is not locked, executing task can lock it
		// Task inherits the priority ceiling of the semaphore
		// Dynamic priority of the task is changed if priority of the task is lower than the ceiling of the semaphore
		
		taskUninstall(EXECTASK);
		semaphoreSetCurrentTask(handle, EXECTASK);
		taskInstall(EXECTASK);
		
		
		//handle->current_task = EXECTASK;
		//handle->task_priority = EXECTASK->effective_priority;
		//if(EXECTASK->effective_priority > handle->ceiling)
		//{
			////Reinstalling task because it has higher priority, so the ready list should be rearranged
			//taskUninstall(EXECTASK);
			//EXECTASK->effective_priority = handle->ceiling;
			//taskInstall(EXECTASK);
		//}
		result = 1;
	}
	else if( EXECTASK != handle->current_task )
	{
		// If the semaphore is locked, executing task cannot lock it
		// Task is put into the waiting queue for the specific semaphore
		// Task is also removed from the ready list
		
		//enqueue(& handle->waiting_tasks, EXECTASK);
		taskUninstall(EXECTASK);
		semaphoreBlockTask(handle, EXECTASK);
		//EXECTASK->blocking_semaphore = handle;
		//if( handle->BLOCKEDLIST == NULL )
		//{
			//handle->BLOCKEDLIST=EXECTASK;
			//handle->BLOCKEDLIST->nexttcb = handle->BLOCKEDLIST;
			//handle->BLOCKEDLIST->prevtcb = handle->BLOCKEDLIST;
		//}
		//else
		//{
			//insert_after(handle->BLOCKEDLIST->prevtcb, EXECTASK);
		//}
		result = 1;
		
	}
	else if( EXECTASK == handle->current_task)
	{
		result = 1;
	}
	
	//ROSA_yield();
	interruptEnable();
	ROSA_yield();
	
	return result;
}

int16_t ROSA_semaphoreUnlock(ROSA_semaphoreHandle_t handle)
{
	interruptDisable();
	int16_t result = 0;
	if( handle->current_task == EXECTASK )
	{
		// If the semaphore is locked, unlock it, change the priority of the task to the last effective priority
		// Set the pointer to the current task to NULL
		taskUninstall(EXECTASK);
		semaphoreUnsetCurrentTask(handle);
		taskInstall(EXECTASK);
		
		result = 1;
	}
	
	if(handle->BLOCKEDLIST != NULL)
	{
		// If there are still some task waiting to take the semaphore, take the first one from the waiting queue
		// Put the task back to ready list
		
		tcb * temp = handle->BLOCKEDLIST;// = handle->BLOCKEDLIST;
		semaphoreUnblockTask( handle, temp );
		semaphoreSetCurrentTask(handle, temp);
		//dequeue(& handle->waiting_tasks, &temp);
		//temp = handle->BLOCKEDLIST;
		//if( handle->BLOCKEDLIST == handle->BLOCKEDLIST->prevtcb )
		//{
			//handle->BLOCKEDLIST=NULL;
		//}
		//else
		//{
			//handle->BLOCKEDLIST=handle->BLOCKEDLIST->nexttcb;
		//}
		//
		//handle->current_task = temp;
		//handle->task_priority = temp->effective_priority;
		//if( temp->effective_priority > handle->ceiling )
		//{
			//temp->effective_priority = handle->ceiling;
		//}
		//
		//temp->blocking_semaphore = NULL;
		taskInstall( temp );
		result = 1;
	}
	
	//ROSA_yield();
	interruptEnable();
	ROSA_yield();
	
	return result;
}

int16_t semaphoreEvictCurrentTask(ROSA_semaphoreHandle_t handle)
{
	int16_t result = 0;
	
	tcb * temp;
	
	// if the semaphore exists and has a task is holding it
	if( handle && handle->current_task != NULL )
	{
		handle->current_task = NULL;
		
		if( handle->BLOCKEDLIST != NULL )
		{
			temp = handle->BLOCKEDLIST;
			semaphoreUnblockTask(handle, temp);
			semaphoreSetCurrentTask(handle, temp);
			taskInstall(handle->current_task);
		}
		
		result = 1;
	}
	
	return result;
}