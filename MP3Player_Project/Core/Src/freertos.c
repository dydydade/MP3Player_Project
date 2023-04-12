/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2023 STMicroelectronics.
  * All rights reserved.
  *
  * This software is licensed under terms that can be found in the LICENSE file
  * in the root directory of this software component.
  * If no LICENSE file comes with this software, it is provided AS-IS.
  *
  ******************************************************************************
  */
/* USER CODE END Header */

/* Includes ------------------------------------------------------------------*/
#include "FreeRTOS.h"
#include "task.h"
#include "main.h"
#include "cmsis_os.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include <stdio.h>
#include "usart.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define WAKEUP_SIGNAL 0x01

#define QUEUE_LENGTH    10
#define QUEUE_ITEM_SIZE sizeof(uint32_t)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
TaskHandle_t vActivateTaskHandle = NULL;
SemaphoreHandle_t xSemaphore;
StaticTask_t xTaskBuffer;
StackType_t xTaskStack[configMINIMAL_STACK_SIZE];

/* USER CODE END Variables */
osThreadId defaultTaskHandle;

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

void vActivateTask(void *pvParameters);
void vTaskManager(void* pvParameters);

/* USER CODE END FunctionPrototypes */

void StartDefaultTask(void const * argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/* GetIdleTaskMemory prototype (linked to static allocation support) */
void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize );

/* USER CODE BEGIN GET_IDLE_TASK_MEMORY */
static StaticTask_t xIdleTaskTCBBuffer;
static StackType_t xIdleStack[configMINIMAL_STACK_SIZE];

void vApplicationGetIdleTaskMemory( StaticTask_t **ppxIdleTaskTCBBuffer, StackType_t **ppxIdleTaskStackBuffer, uint32_t *pulIdleTaskStackSize )
{
  *ppxIdleTaskTCBBuffer = &xIdleTaskTCBBuffer;
  *ppxIdleTaskStackBuffer = &xIdleStack[0];
  *pulIdleTaskStackSize = configMINIMAL_STACK_SIZE;
  /* place for user code */
}
/* USER CODE END GET_IDLE_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
//	setvbuf(stdout, NULL, _IONBF, 0);	// printf
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
	SemaphoreHandle_t currentSongSem = xSemaphoreCreateBinary();
	SemaphoreHandle_t currentVolumeSem = xSemaphoreCreateBinary();
	SemaphoreHandle_t nextSongSem = xSemaphoreCreateBinary();
	SemaphoreHandle_t nextVolumeSem = xSemaphoreCreateBinary();
	SemaphoreHandle_t screenOnSem = xSemaphoreCreateBinary();
	xSemaphore = xSemaphoreCreateBinary();
//	xQueue = (QueueHandle_t)xSemaphore;
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */

  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* definition and creation of defaultTask */
  osThreadDef(defaultTask, StartDefaultTask, osPriorityNormal, 0, 128);
  defaultTaskHandle = osThreadCreate(osThread(defaultTask), NULL);

  /* USER CODE BEGIN RTOS_THREADS */

  if (xSemaphore != NULL) {
	  printf("xSemaphore is not null -- from freertos.c, Manager Task Create\r\n");
	  xTaskCreate(vTaskManager, "Manager", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
	  vTaskStartScheduler();
  }

  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

}

/* USER CODE BEGIN Header_StartDefaultTask */
/**
  * @brief  Function implementing the defaultTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartDefaultTask */
void StartDefaultTask(void const * argument)
{
  /* USER CODE BEGIN StartDefaultTask */
  /* Infinite loop */
  for(;;)
  {
    osDelay(1);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void vActivateTask(void* pvParameters)
{
	printf("Wake Up\r\n");

	vTaskDelay(pdMS_TO_TICKS(5000));
	printf("Fall Asleep\r\n");
	vTaskDelete(NULL);
//	vActivateTaskHandle = NULL;
}

void vTaskManager(void* pvParameters)
{

    while (1) {
    	printf("vTaskManager Start Waiting... Semaphore\r\n");
        if (xSemaphoreTake(xSemaphore, portMAX_DELAY) == pdTRUE) {
        	printf("vTaskManager got Semaphore Successfully!\r\n");
            if (vActivateTaskHandle != NULL) {
            	printf("vActivateTaskHandle is not NULL! Try Delete vActivateTaskHandle!\r\n");
                vTaskDelete(vActivateTaskHandle);
            }

            vActivateTaskHandle = xTaskCreateStatic(
                vActivateTask,
                "ActivateTask",
                configMINIMAL_STACK_SIZE,
                NULL,
				tskIDLE_PRIORITY + 2,
                xTaskStack,
                &xTaskBuffer
            );

        	printf("Create New vActivateTaskHandle Successfully!\r\n");
        	xSemaphoreGive(xSemaphore);
        }
    }
}

/* USER CODE END Application */

