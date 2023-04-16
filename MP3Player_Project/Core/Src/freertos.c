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

#include "dfplayer.h"
#include "fonts.h"
#include "ssd1306.h"
#include "fonts.h"
#include "bitmap.h"
#include <stdio.h>

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define CENTER_BIT (1<<0) // 0x01
#define UP_BIT (1<<1) // 0x02
#define DOWN_BIT (1<<2) // 0x04
#define RIGHT_BIT (1<<3) // 0x08
#define LEFT_BIT (1<<4) // 0x10

#define QUEUE_LENGTH    10
#define QUEUE_ITEM_SIZE sizeof(uint32_t)

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */
TaskHandle_t vActivateTaskHandle = NULL;
EventGroupHandle_t xEventGroup;
SemaphoreHandle_t xSemaphore;
StaticTask_t xTaskBuffer;
StackType_t xTaskStack[configMINIMAL_STACK_SIZE];

uint8_t data[10];
uint8_t pauseStatus;	// 0 : Pause, 1 : Playing -- Music Player
uint8_t activeStatus;	// 0 : Unactivated, 1 : Activated -- LED Pannel
uint8_t currentVolume;  //
uint8_t changeVolume;
uint8_t currentTrackNo;
uint8_t changeTrackNo;
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

/* GetTimerTaskMemory prototype (linked to static allocation support) */
void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize );

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

/* USER CODE BEGIN GET_TIMER_TASK_MEMORY */
static StaticTask_t xTimerTaskTCBBuffer;
static StackType_t xTimerStack[configTIMER_TASK_STACK_DEPTH];

void vApplicationGetTimerTaskMemory( StaticTask_t **ppxTimerTaskTCBBuffer, StackType_t **ppxTimerTaskStackBuffer, uint32_t *pulTimerTaskStackSize )
{
  *ppxTimerTaskTCBBuffer = &xTimerTaskTCBBuffer;
  *ppxTimerTaskStackBuffer = &xTimerStack[0];
  *pulTimerTaskStackSize = configTIMER_TASK_STACK_DEPTH;
  /* place for user code */
}
/* USER CODE END GET_TIMER_TASK_MEMORY */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
	xEventGroup = xEventGroupCreate();
	xSemaphore = xSemaphoreCreateBinary();

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
  xTaskCreate(vTaskManager, "Manager", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY, NULL);
  xTaskCreate(vActivateTask, "ActivateTask", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, &vActivateTaskHandle);

//  if (xEventGroup != NULL) {
//	  xTaskCreate(vTaskManager, "Manager", configMINIMAL_STACK_SIZE, NULL, tskIDLE_PRIORITY + 1, NULL);
//
//  }

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
    osDelay(100);
  }
  /* USER CODE END StartDefaultTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */
void vActivateTask(void* pvParameters)
{
    uint32_t ulNotificationValue;

    for (;;) {
        // Wait for a notification from vTaskManager
        ulNotificationValue = ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

        if (ulNotificationValue > 0) {
            // vActivateTask starts executing...
            // ...
			if (!activeStatus)
			{
				// set activeStatus On
				activeStatus = 1;
				// get every data(volume, song no....etc)
				currentVolume = getCurrentVolume();
				changeVolume = currentVolume;
				currentTrackNo = getCurrentTrackNo();
				changeTrackNo = currentTrackNo;
				// display info
				SSD1306_Display_Volume_Track_Info(currentVolume, changeVolume, currentTrackNo, changeTrackNo);
			}
			else
			{
				// center button : pause/restart music
				if (CENTER_BIT == (uint32_t) ulNotificationValue)
				{
					if (currentVolume != changeVolume || currentTrackNo != changeTrackNo)
					{
						specifyVolume(changeVolume);
						currentVolume = changeVolume;
						HAL_Delay(500);
						specifyTrack(changeTrackNo);
						currentTrackNo = changeTrackNo;
					}
					else
					{
						pauseStatus = !pauseStatus;
						if (pauseStatus)
						{
							pauseMusic();
						}
						else
						{
							restartMusic();
						}
					}

					SSD1306_Display_Volume_Track_Info(currentVolume, changeVolume, currentTrackNo, changeTrackNo);
				}

				// up button : volume up
				if (UP_BIT == (uint32_t) ulNotificationValue)
				{
					if (++changeVolume > 15) changeVolume = 1;
					SSD1306_Display_Volume_Track_Info(currentVolume, changeVolume, currentTrackNo, changeTrackNo);
				}

				// down button : volume down
				if (DOWN_BIT == (uint32_t) ulNotificationValue)
				{
					if (--changeVolume < 1) changeVolume = 15;
					SSD1306_Display_Volume_Track_Info(currentVolume, changeVolume, currentTrackNo, changeTrackNo);
				}

				// right button : next song
				if (RIGHT_BIT == (uint32_t) ulNotificationValue)
				{
					if (++changeTrackNo > 7) changeTrackNo = 1;
					SSD1306_Display_Volume_Track_Info(currentVolume, changeVolume, currentTrackNo, changeTrackNo);
				}

				// left button : previous song
				if (LEFT_BIT == (uint32_t) ulNotificationValue)
				{
					if (--changeTrackNo < 1) changeTrackNo = 7;
					SSD1306_Display_Volume_Track_Info(currentVolume, changeVolume, currentTrackNo, changeTrackNo);
				}
			}

            // Check for a new notification after delay
            if ((ulNotificationValue = ulTaskNotifyTake(pdTRUE, pdMS_TO_TICKS(5000))) == 0) {
                // vActivateTask continues executing after the delay, only if no new notification was received
                // ...
            	// initialize changeVolume, changeTrackNo, activeStatus
            	changeVolume = currentVolume;
				changeTrackNo = currentTrackNo;
				activeStatus = 0;
				SSD1306_Clear();

                continue;

            } else {
                // A new notification was received during the delay, so we skip the rest of the execution
                xTaskNotify(vActivateTaskHandle, ulNotificationValue, eSetValueWithOverwrite);

                continue;
            }
        }
    }
}

void vTaskManager(void* pvParameters)
{

    while (1) {

    	// Waiting Eventbit from IRQHandler
	    EventBits_t activeBit = xEventGroupWaitBits(xEventGroup, 0x1F, pdTRUE, pdFALSE, portMAX_DELAY);

        // Signal vActivateTaskHandle
        xTaskNotify(vActivateTaskHandle, activeBit, eSetValueWithOverwrite);

        // vTaskManager does other work...
        vTaskDelay(pdMS_TO_TICKS(1000));

    }
}


void SSD1306_Display_Volume_Track_Info(uint8_t currVol, uint8_t chngVol, uint8_t currTrack, uint8_t chngTrack)
{
	char currentVolumeChar[3];
	char changeVolumeChar[3];
	char currentTrackNoChar[3];
	char changeTrackNoChar[3];
	itoa(currVol, (char*)currentVolumeChar, 10);
	itoa(chngVol, (char*)changeVolumeChar, 10);
	itoa(currTrack, (char*)currentTrackNoChar, 10);
	itoa(chngTrack, (char*)changeTrackNoChar, 10);

	SSD1306_Clear();
	SSD1306_DrawBitmap(4, 0, Volume_Bitmap, 42, 30, 1);
	SSD1306_DrawBitmap(2, 34, Music_Bitmap, 42, 30, 1);

	SSD1306_GotoXY(42, 5);
	SSD1306_Puts("CURR VOL :", &Font_7x10, 1);
	SSD1306_GotoXY(111, 5);
	SSD1306_Puts(currentVolumeChar, &Font_7x10, 1);

	SSD1306_GotoXY(42, 15);
	SSD1306_Puts("CHNG VOL :", &Font_7x10, 1);
	SSD1306_GotoXY(111, 15);
	SSD1306_Puts(changeVolumeChar, &Font_7x10, 1);

	SSD1306_GotoXY(42, 39);
	SSD1306_Puts("CURR SONG:", &Font_7x10, 1);
	SSD1306_GotoXY(111, 39);
	SSD1306_Puts(currentTrackNoChar, &Font_7x10, 1);

	SSD1306_GotoXY(42, 49);
	SSD1306_Puts("CHNG SONG:", &Font_7x10, 1);
	SSD1306_GotoXY(111, 49);
	SSD1306_Puts(changeTrackNoChar, &Font_7x10, 1);

	SSD1306_UpdateScreen(); // display
}
/* USER CODE END Application */

