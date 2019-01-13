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
ROSA_taskHandle_t task4_handle;
ROSA_taskHandle_t arbiter_task_handle;

ROSA_taskHandle_t light1_handle = NULL;
ROSA_taskHandle_t light2_handle = NULL;

ROSA_semaphoreHandle_t S1 = NULL;
ROSA_semaphoreHandle_t S2 = NULL;
ROSA_semaphoreHandle_t S3 = NULL;
ROSA_semaphoreHandle_t S4 = NULL;

ROSA_taskHandle_t task_handle_1;
ROSA_taskHandle_t task_handle_2;
ROSA_taskHandle_t task_handle_3;
ROSA_taskHandle_t task_handle_4;
ROSA_taskHandle_t task_handle_5;
ROSA_taskHandle_t task_handle_6;
ROSA_taskHandle_t task_handle_7;
ROSA_taskHandle_t task_handle_8;
ROSA_taskHandle_t task_handle_9;
ROSA_taskHandle_t task_handle_10;
ROSA_taskHandle_t task_handle_11;
ROSA_taskHandle_t task_handle_12;
ROSA_taskHandle_t task_handle_13;
ROSA_taskHandle_t task_handle_14;
ROSA_taskHandle_t task_handle_15;
ROSA_taskHandle_t task_handle_16;
ROSA_taskHandle_t task_handle_17;
ROSA_taskHandle_t task_handle_18;
ROSA_taskHandle_t task_handle_19;
ROSA_taskHandle_t task_handle_20;
ROSA_taskHandle_t task_handle_21;
ROSA_taskHandle_t task_handle_22;
ROSA_taskHandle_t task_handle_23;
ROSA_taskHandle_t task_handle_24;
ROSA_taskHandle_t task_handle_25;
ROSA_taskHandle_t task_handle_26;
ROSA_taskHandle_t task_handle_27;
ROSA_taskHandle_t task_handle_28;
ROSA_taskHandle_t task_handle_29;
ROSA_taskHandle_t task_handle_30;
ROSA_taskHandle_t task_handle_31;
ROSA_taskHandle_t task_handle_32;

void init_tc()
{
	*((unsigned int*)0xFFFF3844) = 0x0000C002;				// set waveform options
	*((unsigned int*)0xFFFF385C) = 0x000036D0;				// initialize CV to 0
	*((unsigned int*)0xFFFF3840) = 0x00000004 | 0x00000001; //start
}

void toggle_1(void){ while(1){ledToggle(LED2_GPIO); /* ROSA_delay(100); */} }                                                     
void toggle_2(void){ while(1){ledToggle(LED3_GPIO); /* ROSA_delay(100); */} }                                                     
void toggle_3(void){ while(1){ledToggle(LED4_GPIO); /* ROSA_delay(100); */} }                                                     
void toggle_4(void){ while(1){ledToggle(LED5_GPIO); /* ROSA_delay(100); */} }                                                     
void toggle_5(void){ while(1){ledToggle(LED6_GPIO); /* ROSA_delay(100); */} }                                                     
void toggle_6(void){ while(1){ledToggle(LED1_GPIO); /* ROSA_delay(100); */} }                                                     
void toggle_7(void){ while(1){ledToggle(LED2_GPIO); /* ROSA_delay(100); */} }                                                     
void toggle_8(void){ while(1){ledToggle(LED3_GPIO); /* ROSA_delay(100); */} }                                                     
void toggle_9(void){ while(1){ledToggle(LED4_GPIO); /* ROSA_delay(100); */} }                                                     
void toggle_10(void){ while(1){ledToggle(LED5_GPIO); /* ROSA_delay(100); */} }                                                    
void toggle_11(void){ while(1){ledToggle(LED6_GPIO); /* ROSA_delay(100); */} }                                                    
void toggle_12(void){ while(1){ledToggle(LED1_GPIO); /* ROSA_delay(100); */} }                                                    
void toggle_13(void){ while(1){ledToggle(LED2_GPIO); /* ROSA_delay(100); */} }                                                    
void toggle_14(void){ while(1){ledToggle(LED3_GPIO); /* ROSA_delay(100); */} }                                                    
void toggle_15(void){ while(1){ledToggle(LED4_GPIO); /* ROSA_delay(100); */} }                                                    
void toggle_16(void){ while(1){ledToggle(LED5_GPIO); /* ROSA_delay(100); */} }                                                    
void toggle_17(void){ while(1){ledToggle(LED6_GPIO); /* ROSA_delay(100); */} }                                                    
void toggle_18(void){ while(1){ledToggle(LED1_GPIO); /* ROSA_delay(100); */} }                                                    
void toggle_19(void){ while(1){ledToggle(LED2_GPIO); /* ROSA_delay(100); */} }                                                    
void toggle_20(void){ while(1){ledToggle(LED3_GPIO); /* ROSA_delay(100); */} }                                                    
void toggle_21(void){ while(1){ledToggle(LED4_GPIO); /* ROSA_delay(100); */} }                                                    
void toggle_22(void){ while(1){ledToggle(LED5_GPIO); /* ROSA_delay(100); */} }                                                    
void toggle_23(void){ while(1){ledToggle(LED6_GPIO); /* ROSA_delay(100); */} }                                                    
void toggle_24(void){ while(1){ledToggle(LED1_GPIO); /* ROSA_delay(100); */} }                                                    
void toggle_25(void){ while(1){ledToggle(LED2_GPIO); /* ROSA_delay(100); */} }                                                    
void toggle_26(void){ while(1){ledToggle(LED3_GPIO); /* ROSA_delay(100); */} }                                                    
void toggle_27(void){ while(1){ledToggle(LED4_GPIO); /* ROSA_delay(100); */} }                                                    
void toggle_28(void){ while(1){ledToggle(LED5_GPIO); /* ROSA_delay(100); */} }                                                    
void toggle_29(void){ while(1){ledToggle(LED6_GPIO); /* ROSA_delay(100); */} }                                                    
void toggle_30(void){ while(1){ledToggle(LED1_GPIO); /* ROSA_delay(100); */} }                                                    
void toggle_31(void){ while(1){ledToggle(LED2_GPIO); /* ROSA_delay(100); */} }                                                    
void toggle_32(void){ while(1){ledToggle(LED3_GPIO); /* ROSA_delay(100); */} }  

void hog_semaphores_task(void)
{
	while(1)
	{
		ledOn(LED6_GPIO);
		ROSA_semaphoreLock(S4);
		ROSA_delay(300);
		ROSA_semaphoreLock(S1);
		ROSA_delay(300);
		ROSA_semaphoreLock(S2);
		ROSA_delay(300);
		ROSA_semaphoreLock(S3);
		ROSA_delay(300);
		ledOff(LED6_GPIO);
		ROSA_delay(800);
		
		ROSA_semaphoreUnlock(S3);
		ROSA_semaphoreUnlock(S2);
		ROSA_semaphoreUnlock(S1);
		ROSA_semaphoreUnlock(S4);
		//ledOff(LED6_GPIO);
		
		ROSA_delay(800);
		
		//hog_semaphores_task_handle = NULL;
		//ROSA_taskDelete(hog_semaphores_task_handle);
	}
}

void task1(void)
{
	uint64_t time = ROSA_getTickCount();
	while(1)
	{
		ROSA_semaphoreLock(S1);
		ledToggle(LED0_GPIO);
		time = ROSA_getTickCount();
		ROSA_delay(100);
		ROSA_semaphoreUnlock(S1);
	}
}

void task2(void)
{
	ROSA_delay(5);
	uint64_t time = ROSA_getTickCount();
	while(1)
	{
		ROSA_semaphoreLock(S2);
		ledToggle(LED1_GPIO);
		time = ROSA_getTickCount();
		ROSA_delay(100);
		ROSA_semaphoreUnlock(S2);
	}
}

void task3(void)
{
	ROSA_delay(10);
	uint64_t time = ROSA_getTickCount();
	while(1)
	{
		ROSA_semaphoreLock(S3);
		ledToggle(LED2_GPIO);
		time = ROSA_getTickCount();
		ROSA_delay(100);
		ROSA_semaphoreUnlock(S3);
	}
}

void task4(void)
{
	ROSA_delay(15);
	uint64_t time = ROSA_getTickCount();
	while(1)
	{
		ROSA_semaphoreLock(S4);
		ledToggle(LED3_GPIO);
		time = ROSA_getTickCount();
		ROSA_delay(100);
		ROSA_semaphoreUnlock(S4);
	}
}

void arbiter_task(void)
{
	while(1)
	{
		ROSA_delay(2000);
		if( hog_semaphores_task_handle == NULL )
		{
			ROSA_taskCreate(& hog_semaphores_task_handle, "hogS", hog_semaphores_task, STACK_SIZE, 3);
		}
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

ROSA_taskHandle_t semaphores_task_handle     = NULL;
ROSA_taskHandle_t led3_task_handle = NULL;
ROSA_taskHandle_t led5_task_handle = NULL;
ROSA_taskHandle_t led7_task_handle = NULL;

void semaphores_task(void)
{
	while(1)
	{
		ROSA_semaphoreLock(S3);
		ledOn(LED0_GPIO);
		delay_ms(1000);
		
		ROSA_semaphoreLock(S2);
		ledOn(LED1_GPIO);
		delay_ms(1000);

		ROSA_semaphoreLock(S1);
		ledOn(LED2_GPIO);
		delay_ms(1000);
		
		ledOff(LED2_GPIO);
		ROSA_semaphoreUnlock(S1);
		delay_ms(1000);

		ledOff(LED1_GPIO);
		ROSA_semaphoreUnlock(S2);
		delay_ms(1000);
		
		ledOff(LED0_GPIO);
		ROSA_semaphoreUnlock(S3);
		delay_ms(1000);
	}
}

void led3_task(void)
{
	while(1)
	{
		ledToggle(LED3_GPIO);
		ROSA_delay(100);
	}
}

void led5_task(void)
{
	while(1)
	{
		ledToggle(LED5_GPIO);
		ROSA_delay(100);
	}
}

void led7_task(void)
{
	while(1)
	{
		ledToggle(LED7_GPIO);
		ROSA_delay(100);
	}
}

/*************************************************************
 * Main function
 ************************************************************/
int main(void)
{
	//Initialize the ROSA kernel
	ROSA_init();
	
	//ROSA_taskCreate(& semaphores_task_handle, "tsk1", semaphores_task, 0x40, 7);
	//ROSA_taskCreate(& led3_task_handle, "led3", led3_task, 0x40, 2);
	//ROSA_taskCreate(& led5_task_handle, "led5", led5_task, 0x40, 4);
	//ROSA_taskCreate(& led7_task_handle, "led7", led7_task, 0x40, 6);
	//ROSA_semaphoreCreate(&S1,1);
	//ROSA_semaphoreCreate(&S2,3);
	//ROSA_semaphoreCreate(&S3,5);
	
	//ROSA_taskCreate(& hog_semaphores_task_handle, "hogS", hog_semaphores_task, STACK_SIZE, 2);
	ROSA_taskCreate(& task1_handle,					"tsk1", task1, STACK_SIZE, 3);
	ROSA_taskCreate(& task2_handle,					"tsk2", task2, STACK_SIZE, 3);
	ROSA_taskCreate(& task3_handle,					"tsk3", task3, STACK_SIZE, 3);
	ROSA_taskCreate(& task4_handle,					"tsk4", task4, STACK_SIZE, 3);
	ROSA_taskCreate(& arbiter_task_handle,			"arbt", arbiter_task, STACK_SIZE, 1);
	ROSA_semaphoreCreate(& S1, 3);
	ROSA_semaphoreCreate(& S2, 3);
	ROSA_semaphoreCreate(& S3, 3);
	ROSA_semaphoreCreate(& S4, 3);
	
	//ROSA_taskCreate(& task_handle_1, "fsdj", toggle_1, STACK_SIZE, 3);
	//ROSA_taskCreate(& task_handle_2, "test", toggle_2, STACK_SIZE, 3);
	//ROSA_taskCreate(& task_handle_3, "test", toggle_3, STACK_SIZE, 3);
	//ROSA_taskCreate(& task_handle_4, "test", toggle_4, STACK_SIZE, 3);
	//ROSA_taskCreate(& task_handle_5, "test", toggle_5, STACK_SIZE, 3);
	//ROSA_taskCreate(& task_handle_6, "test", toggle_6, STACK_SIZE, 3);
	//ROSA_taskCreate(& task_handle_7, "test", toggle_7, STACK_SIZE, 3);
	//ROSA_taskCreate(& task_handle_8, "test", toggle_8, STACK_SIZE, 3);
	//ROSA_taskCreate(& task_handle_9, "test", toggle_9, STACK_SIZE, 3);
	//ROSA_taskCreate(& task_handle_10, "test", toggle_10, STACK_SIZE, 3);
	//ROSA_taskCreate(& task_handle_11, "test", toggle_11, STACK_SIZE, 3);
	//ROSA_taskCreate(& task_handle_12, "test", toggle_12, STACK_SIZE, 3);
	//ROSA_taskCreate(& task_handle_13, "test", toggle_13, STACK_SIZE, 3);
	//ROSA_taskCreate(& task_handle_14, "test", toggle_14, STACK_SIZE, 3);
	//ROSA_taskCreate(& task_handle_15, "test", toggle_15, STACK_SIZE, 3);
	//ROSA_taskCreate(& task_handle_16, "test", toggle_16, STACK_SIZE, 3);
	//ROSA_taskCreate(& task_handle_17, "test", toggle_17, STACK_SIZE, 3);
	//ROSA_taskCreate(& task_handle_18, "test", toggle_18, STACK_SIZE, 3);
	//ROSA_taskCreate(& task_handle_19, "test", toggle_19, STACK_SIZE, 3);
	//ROSA_taskCreate(& task_handle_20, "test", toggle_20, STACK_SIZE, 3);
	//ROSA_taskCreate(& task_handle_21, "test", toggle_21, STACK_SIZE, 3);
	//ROSA_taskCreate(& task_handle_22, "test", toggle_22, STACK_SIZE, 3);
	//ROSA_taskCreate(& task_handle_23, "test", toggle_23, STACK_SIZE, 3);
	//ROSA_taskCreate(& task_handle_24, "test", toggle_24, STACK_SIZE, 3);
	//ROSA_taskCreate(& task_handle_25, "test", toggle_25, STACK_SIZE, 3);
	//ROSA_taskCreate(& task_handle_26, "test", toggle_26, STACK_SIZE, 3);
	//ROSA_taskCreate(& task_handle_27, "test", toggle_27, STACK_SIZE, 3);
	//ROSA_taskCreate(& task_handle_28, "test", toggle_28, STACK_SIZE, 3);
	//ROSA_taskCreate(& task_handle_29, "test", toggle_29, STACK_SIZE, 3);
	//ROSA_taskCreate(& task_handle_30, "test", toggle_30, STACK_SIZE, 3);
	//ROSA_taskCreate(& task_handle_31, "test", toggle_31, STACK_SIZE, 3);
	//ROSA_taskCreate(& task_handle_32, "test", toggle_32, STACK_SIZE, 3);
	
	init_tc();
	
	timerStart();
	ROSA_start();
	/* Execution will never return here */
	while(1);
}
