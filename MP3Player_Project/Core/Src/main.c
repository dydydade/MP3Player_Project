/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * @file           : main.c
  * @brief          : Main program body
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
#include "main.h"
#include "cmsis_os.h"
#include "i2c.h"
#include "usart.h"
#include "gpio.h"

/* Private includes ----------------------------------------------------------*/
/* USER CODE BEGIN Includes */
#include "dfplayer.h"
#include "fonts.h"
#include "ssd1306.h"
#include "fonts.h"
#include <stdio.h>

#include "portmacro.h"
#include "projdefs.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */
#define Start_Byte 0x7E             //
#define Version_Byte 0xFF           // 버전
#define Command_Length 0x06        // 길이
#define End_Byte 0xEF              // 종료
#define Acknowledge 0x00           //

#define CENTER_BIT (1<<0) // 0x01
#define UP_BIT (1<<1) // 0x02
#define DOWN_BIT (1<<2) // 0x04
#define RIGHT_BIT (1<<3) // 0x08
#define LEFT_BIT (1<<4) // 0x10

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/

/* USER CODE BEGIN PV */

extern TaskHandle_t vActivateTaskHandle;
extern EventGroupHandle_t xEventGroup;
extern StaticTask_t xTaskBuffer;
extern StackType_t xTaskStack[configMINIMAL_STACK_SIZE];

extern uint8_t data[10];
extern uint8_t pauseStatus;
extern uint8_t activeStatus;
extern uint8_t currentVolume; // default volume
extern uint8_t changeVolume;
extern uint8_t currentTrackNo;
extern uint8_t changeTrackNo;

extern void vActivateTask(void* pvParameters);

/* USER CODE END PV */

/* Private function prototypes -----------------------------------------------*/
void SystemClock_Config(void);
void MX_FREERTOS_Init(void);
static void MX_NVIC_Init(void);
/* USER CODE BEGIN PFP */

/* USER CODE END PFP */

/* Private user code ---------------------------------------------------------*/
/* USER CODE BEGIN 0 */

/* USER CODE END 0 */

/**
  * @brief  The application entry point.
  * @retval int
  */
int main(void)
{
  /* USER CODE BEGIN 1 */

  /* USER CODE END 1 */

  /* MCU Configuration--------------------------------------------------------*/

  /* Reset of all peripherals, Initializes the Flash interface and the Systick. */
  HAL_Init();

  /* USER CODE BEGIN Init */

  /* USER CODE END Init */

  /* Configure the system clock */
  SystemClock_Config();

  /* USER CODE BEGIN SysInit */

  /* USER CODE END SysInit */

  /* Initialize all configured peripherals */
  MX_GPIO_Init();
  MX_USART1_UART_Init();
  MX_I2C1_Init();
  MX_USART3_UART_Init();

  /* Initialize interrupts */
  MX_NVIC_Init();
  /* USER CODE BEGIN 2 */
  SSD1306_Init();

  SSD1306_GotoXY(0, 10);
  SSD1306_Puts("HELLO", &Font_11x18, 1);
  SSD1306_GotoXY(10, 30);
  SSD1306_Puts(" WORLD! :)", &Font_11x18, 1);
  SSD1306_UpdateScreen();  // display
  HAL_Delay(2500);		   // 로고
  specifyVolume(4);
  HAL_Delay(500);
  specifyTrack(1);
  HAL_Delay(500);
  /* USER CODE END 2 */

  /* Call init function for freertos objects (in freertos.c) */
  MX_FREERTOS_Init();

  /* Start scheduler */
  vTaskStartScheduler();

  /* We should never get here as control is now taken by the scheduler */
  /* Infinite loop */
  /* USER CODE BEGIN WHILE */

  while (1)
  {

    /* USER CODE END WHILE */

    /* USER CODE BEGIN 3 */
  }
  /* USER CODE END 3 */
}

/**
  * @brief System Clock Configuration
  * @retval None
  */
void SystemClock_Config(void)
{
  RCC_OscInitTypeDef RCC_OscInitStruct = {0};
  RCC_ClkInitTypeDef RCC_ClkInitStruct = {0};

  /** Initializes the RCC Oscillators according to the specified parameters
  * in the RCC_OscInitTypeDef structure.
  */
  RCC_OscInitStruct.OscillatorType = RCC_OSCILLATORTYPE_HSE;
  RCC_OscInitStruct.HSEState = RCC_HSE_ON;
  RCC_OscInitStruct.HSEPredivValue = RCC_HSE_PREDIV_DIV1;
  RCC_OscInitStruct.HSIState = RCC_HSI_ON;
  RCC_OscInitStruct.PLL.PLLState = RCC_PLL_ON;
  RCC_OscInitStruct.PLL.PLLSource = RCC_PLLSOURCE_HSE;
  RCC_OscInitStruct.PLL.PLLMUL = RCC_PLL_MUL2;
  if (HAL_RCC_OscConfig(&RCC_OscInitStruct) != HAL_OK)
  {
    Error_Handler();
  }

  /** Initializes the CPU, AHB and APB buses clocks
  */
  RCC_ClkInitStruct.ClockType = RCC_CLOCKTYPE_HCLK|RCC_CLOCKTYPE_SYSCLK
                              |RCC_CLOCKTYPE_PCLK1|RCC_CLOCKTYPE_PCLK2;
  RCC_ClkInitStruct.SYSCLKSource = RCC_SYSCLKSOURCE_PLLCLK;
  RCC_ClkInitStruct.AHBCLKDivider = RCC_SYSCLK_DIV1;
  RCC_ClkInitStruct.APB1CLKDivider = RCC_HCLK_DIV1;
  RCC_ClkInitStruct.APB2CLKDivider = RCC_HCLK_DIV1;

  if (HAL_RCC_ClockConfig(&RCC_ClkInitStruct, FLASH_LATENCY_0) != HAL_OK)
  {
    Error_Handler();
  }
}

/**
  * @brief NVIC Configuration.
  * @retval None
  */
static void MX_NVIC_Init(void)
{
  /* EXTI1_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI1_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI1_IRQn);
  /* EXTI2_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI2_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI2_IRQn);
  /* EXTI3_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI3_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI3_IRQn);
  /* EXTI4_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI4_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI4_IRQn);
  /* EXTI9_5_IRQn interrupt configuration */
  HAL_NVIC_SetPriority(EXTI9_5_IRQn, 5, 0);
  HAL_NVIC_EnableIRQ(EXTI9_5_IRQn);
}

/* USER CODE BEGIN 4 */

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
	static uint32_t beforeTick = 0;

    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    // center button : pause/restart music
	if(GPIO_Pin == GPIO_PIN_1)
	{
		if(HAL_GetTick() - beforeTick >= 200)
		{
			beforeTick = HAL_GetTick();

			printf("you pressed center button!\r\n");

			if (xEventGroup != NULL) {
				xEventGroupSetBitsFromISR(xEventGroup, CENTER_BIT, &xHigherPriorityTaskWoken);
			}

			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);

		}
	}

    // up button : volume up
	if(GPIO_Pin == GPIO_PIN_2)
	{

		if(HAL_GetTick() - beforeTick >= 200)
		{
			beforeTick = HAL_GetTick();

			printf("you pressed up button!\r\n");

			if (xEventGroup != NULL) {
				xEventGroupSetBitsFromISR(xEventGroup, UP_BIT, &xHigherPriorityTaskWoken);
			}

			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}

	}

    // down button : volume down
	if(GPIO_Pin == GPIO_PIN_3)
	{
		if(HAL_GetTick() - beforeTick >= 200)
		{
			beforeTick = HAL_GetTick();

			printf("you pressed down button!\r\n");

			if (xEventGroup != NULL) {
				xEventGroupSetBitsFromISR(xEventGroup, DOWN_BIT, &xHigherPriorityTaskWoken);
			}

			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
	}

	// right button : next song
	if(GPIO_Pin == GPIO_PIN_4)
	{
		if(HAL_GetTick() - beforeTick >= 200)
		{
			beforeTick = HAL_GetTick();

			printf("you pressed right button!\r\n");

			if (xEventGroup != NULL) {
				xEventGroupSetBitsFromISR(xEventGroup, RIGHT_BIT, &xHigherPriorityTaskWoken);
			}

			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
	}

	// left button : previous song
	if(GPIO_Pin == GPIO_PIN_5)
	{
		if(HAL_GetTick() - beforeTick >= 200)
		{
			beforeTick = HAL_GetTick();

			printf("you pressed left button!\r\n");

			if (xEventGroup != NULL) {
				xEventGroupSetBitsFromISR(xEventGroup, LEFT_BIT, &xHigherPriorityTaskWoken);
			}

			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
		}
	}
}


/* USER CODE END 4 */

/**
  * @brief  Period elapsed callback in non blocking mode
  * @note   This function is called  when TIM4 interrupt took place, inside
  * HAL_TIM_IRQHandler(). It makes a direct call to HAL_IncTick() to increment
  * a global variable "uwTick" used as application time base.
  * @param  htim : TIM handle
  * @retval None
  */
void HAL_TIM_PeriodElapsedCallback(TIM_HandleTypeDef *htim)
{
  /* USER CODE BEGIN Callback 0 */

  /* USER CODE END Callback 0 */
  if (htim->Instance == TIM4) {
    HAL_IncTick();
  }
  /* USER CODE BEGIN Callback 1 */

  /* USER CODE END Callback 1 */
}

/**
  * @brief  This function is executed in case of error occurrence.
  * @retval None
  */
void Error_Handler(void)
{
  /* USER CODE BEGIN Error_Handler_Debug */
  /* User can add his own implementation to report the HAL error return state */
  __disable_irq();
  while (1)
  {
  }
  /* USER CODE END Error_Handler_Debug */
}

#ifdef  USE_FULL_ASSERT
/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t *file, uint32_t line)
{
  /* USER CODE BEGIN 6 */
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */
  /* USER CODE END 6 */
}
#endif /* USE_FULL_ASSERT */
