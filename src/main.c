//Standard library includes
#include <avr32/io.h>

//Kernel includes
#include "kernel/rosa_ker.h"
#include "include/kernel/rosa_tim.h"
#include "include/kernel/semaphore.h"

//Driver includes
#include "drivers/led.h"
#include "drivers/delay.h"
#include "drivers/usart.h"


//Include configuration
#include "rosa_config.h"

 void taskLo(void);
 void taskMi(void);
 void taskHi(void);

 void ROSA_test_delete_task_semaphore(void);

#define TASK_MAX_STACK 0x40

// Task handle declarations
ROSA_taskHandle_t t_taskLo = NULL; 		// Low priority task
ROSA_taskHandle_t t_taskMi = NULL;		// Medium priority task
ROSA_taskHandle_t t_taskHi = NULL;		// High priority task

// Semaphore handle declarations
ROSA_semaphoreHandle_t s_sem;			// A semaphore to be used by tasks L and H

// This task locks a semaphore and goes to sleep
void taskLo(void)
{
	while(1) {
		ROSA_semaphoreLock(s_sem);
		ledOn(LED1_GPIO);
		ROSA_delay(50);
		ROSA_semaphoreUnlock(s_sem);

		// Task was not deleted, so turn LED1 off
		ledOff(LED1_GPIO);
		ROSA_delay(1000);
	}

}

// This task will try to delete taskLo who is currently holding the semaphore
void taskMi(void)
{
	while(1) {
		ROSA_delay(20);
		
		// Try to delete task
		if(ROSA_taskDelete(t_taskLo) < 0) {
			// Task delete failed, could not delete a task holding a semaphore
			ledOn(LED2_GPIO);
			
			// Try to delete the semaphore instead
			if (ROSA_semaphoreDelete(s_sem) >= 0) {
				ledOn(LED4_GPIO);
			}

			} else {
			// Task delete successfull, can delete tasks holding semaphores
			ledOn(LED3_GPIO);
			t_taskLo = NULL;
		}
		
		ROSA_delay(10000);
	}

}

// This task will try to lock the same semaphore as Lo, if it succeeds it will turn on LED7
void taskHi(void)
{
	while(1) {
		ROSA_delay(10);

		// Try to lock the semaphore, but get blocked
		ROSA_semaphoreLock(s_sem);

		// Succeeded to get the semaphore, it did not get indefinitely blocked.
		ledOn(LED7_GPIO);
		ROSA_semaphoreUnlock(s_sem);

		// Delete the tasks, test's done
		//ROSA_taskDelete(t_taskMi);
		
		ROSA_taskDelete(NULL);	// Since this is NULL, it should delete the running task (taskHi)

		ledOn(LED6_GPIO); // Did not delete this task
	}

}

void ROSA_test_delete_task_semaphore(void)
{
	// Create the tasks
	ROSA_taskCreate(&t_taskLo, "tskL", taskLo, TASK_MAX_STACK, 3);
	ROSA_taskCreate(&t_taskMi, "tskM", taskMi, TASK_MAX_STACK, 2);
	ROSA_taskCreate(&t_taskHi, "tskH", taskHi, TASK_MAX_STACK, 1);

	// Create the semaphore
	ROSA_semaphoreCreate(&s_sem, 1);
}

/*************************************************************
 * Main function
 ************************************************************/
int main(void)
{
	//Initialize the ROSA kernel
	ROSA_init();
	
	ROSA_test_delete_task_semaphore();
	
	timerStart();
	ROSA_start();
	/* Execution will never return here */
	while(1);
}
