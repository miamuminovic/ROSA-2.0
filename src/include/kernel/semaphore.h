#ifndef SEMAPHORE_H_
#define SEMAPHORE_H_

#include "include/kernel/rosa_ker.h"

typedef struct semaphore_t
{
	uint8_t ceiling;
	uint8_t task_priority;
	tcb * current_task;
	tcb * BLOCKEDLIST;
	
	struct semaphore_t * nextsemaphore;
	struct semaphore_t * prevsemaphore;
} semaphore;

#define ROSA_semaphoreHandle_t semaphore*

extern semaphore * SEMAPHORES;

int16_t ROSA_semaphoreCreate(ROSA_semaphoreHandle_t * handle, uint8_t ceiling);
int16_t ROSA_semaphoreDelete(ROSA_semaphoreHandle_t handle);
int16_t ROSA_semaphorePeek(ROSA_semaphoreHandle_t handle);
int16_t ROSA_semaphoreLock(ROSA_semaphoreHandle_t handle);
int16_t ROSA_semaphoreUnlock(ROSA_semaphoreHandle_t handle);

int16_t semaphoreAdd(ROSA_semaphoreHandle_t handle);
int16_t semaphoreEvictCurrentTask(ROSA_semaphoreHandle_t handle);					// used for deleting a task that is holding a semaphore
int16_t semaphore_insert_after(semaphore * position, semaphore * new_semaphore);
int16_t semaphore_remove(semaphore * removed_semaphore);
int16_t semaphoreBlockTask  ( semaphore * semaphore_handle, tcb * new_task );
int16_t semaphoreUnblockTask( semaphore * semaphore_handle, tcb * new_task );
int16_t semaphoreSetCurrentTask( semaphore * semaphore_handle, tcb * task_handle );
int16_t semaphoreUnsetCurrentTask( semaphore * semaphore_handle );

#endif /* SEMAPHORE_H_ */