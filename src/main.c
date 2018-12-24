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

#define STACK_SIZE 0x40

ROSA_taskHandle_t hog_semaphores_task_handle;
ROSA_taskHandle_t task1_handle;
ROSA_taskHandle_t task2_handle;
ROSA_taskHandle_t task3_handle;
ROSA_taskHandle_t arbiter_task_handle;

ROSA_taskHandle_t light1_handle = NULL;
ROSA_taskHandle_t light2_handle = NULL;

ROSA_semaphoreHandle_t S1 = NULL;
ROSA_semaphoreHandle_t S2 = NULL;
ROSA_semaphoreHandle_t S3 = NULL;
ROSA_semaphoreHandle_t S4 = NULL;

void hog_semaphores_task(void)
{
	while(1)
	{
		ROSA_semaphoreLock(S4);
		ROSA_delay(300);
		ROSA_semaphoreLock(S1);
		ROSA_delay(300);
		ROSA_semaphoreLock(S2);
		ROSA_delay(300);
		ROSA_semaphoreLock(S3);
		ROSA_delay(300);
		ROSA_semaphoreUnlock(S4);
		ROSA_delay(1000);
		ROSA_semaphoreUnlock(S3);
		ROSA_semaphoreUnlock(S2);
		ROSA_semaphoreUnlock(S1);
		ROSA_semaphoreUnlock(S4);
	}
}

void task1(void)
{
	uint64_t time = ROSA_getTickCount();
	while(1)
	{
		ROSA_semaphoreLock(S1);
		
		if( ROSA_getTickCount() - time < 1000 )
		{
			ledToggle(LED0_GPIO);
		}
		else
		{
			ledOff(LED0_GPIO);
			time = ROSA_getTickCount();
			ROSA_semaphoreUnlock(S1);
		}
		ROSA_delay(100);
	}
}

void task2(void)
{
	uint64_t time = ROSA_getTickCount();
	while(1)
	{
		ROSA_semaphoreLock(S2);
		
		if( ROSA_getTickCount() - time < 1000 )
		{
			ledToggle(LED1_GPIO);
		}
		else
		{
			ledOff(LED1_GPIO);
			time = ROSA_getTickCount();
			ROSA_semaphoreUnlock(S2);
		}
		ROSA_delay(100);
	}
}

void task3(void)
{
	uint64_t time = ROSA_getTickCount();
	while(1)
	{
		ROSA_semaphoreLock(S3);
		
		if( ROSA_getTickCount() - time < 1000 )
		{
			ledToggle(LED2_GPIO);
		}
		else
		{
			ledOff(LED2_GPIO);
			time = ROSA_getTickCount();
			ROSA_semaphoreUnlock(S3);
		}
		ROSA_delay(100);
	}
}

void arbiter_task(void)
{
	while(1)
	{
		//ROSA_delay(10);
		//ROSA_semaphoreLock(S4);
		ROSA_delay(5000);
		if( hog_semaphores_task_handle != NULL)
		{
			ROSA_taskDelete(hog_semaphores_task_handle);
			hog_semaphores_task_handle = NULL;
		}
		else
		{
			ROSA_taskCreate(& hog_semaphores_task_handle, "hogS", hog_semaphores_task, STACK_SIZE, 1);
		}
		
		//ROSA_delay(5000);
		//ROSA_semaphoreUnlock(S4);
		//ROSA_delay(10);
	}
}

void light1(void)
{
	uint64_t time = ROSA_getTickCount();
	while(1)
	{
		ledOn(LED0_GPIO);
		ledOff(LED1_GPIO);
		
		ROSA_delayUntil(& time, 500);
	}
}

void light2(void)
{
	uint64_t time = ROSA_getTickCount() + 250;
	while(1)
	{
		ledOn(LED1_GPIO);
		ledOff(LED0_GPIO);
		
		ROSA_delayUntil(& time, 500);
	}
}

/*************************************************************
 * Main function
 ************************************************************/
int main(void)
{
	//Initialize the ROSA kernel
	ROSA_init();
	
	ROSA_taskCreate(& hog_semaphores_task_handle, "hogS", hog_semaphores_task, STACK_SIZE, 2);
	ROSA_taskCreate(& task1_handle,					"tsk1", task1, STACK_SIZE, 3);
	ROSA_taskCreate(& task2_handle,					"tsk2", task2, STACK_SIZE, 3);
	ROSA_taskCreate(& task3_handle,					"tsk3", task3, STACK_SIZE, 3);
	//ROSA_taskCreate(& arbiter_task_handle,			"arbt", arbiter_task, STACK_SIZE, 1);
	
	//ROSA_taskCreate(& light1_handle, "lgt1", light1, STACK_SIZE, 1);
	//ROSA_taskCreate(& light2_handle, "lgt2", light2, STACK_SIZE, 1);
	
	ROSA_semaphoreCreate(& S1, 2);
	ROSA_semaphoreCreate(& S2, 2);
	ROSA_semaphoreCreate(& S3, 2);
	ROSA_semaphoreCreate(& S4, 1);
	
	timerStart();
	ROSA_start();
	/* Execution will never return here */
	while(1);
}
