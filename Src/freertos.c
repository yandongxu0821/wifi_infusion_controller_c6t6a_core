/* USER CODE BEGIN Header */
/**
  ******************************************************************************
  * File Name          : freertos.c
  * Description        : Code for freertos applications
  ******************************************************************************
  * @attention
  *
  * Copyright (c) 2026 STMicroelectronics.
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

#include "ssd1315.h"
#include "bh1750.h"
#include "state.h"
#include "handshake.h"
#include "stdio.h"
#include "usart.h"
#include "string.h"
#include "stdlib.h"

/* USER CODE END Includes */

/* Private typedef -----------------------------------------------------------*/
/* USER CODE BEGIN PTD */

/* USER CODE END PTD */

/* Private define ------------------------------------------------------------*/
/* USER CODE BEGIN PD */

#define REPORT_INTERVAL 15000
#define UART_TIMEOUT 10000

/* USER CODE END PD */

/* Private macro -------------------------------------------------------------*/
/* USER CODE BEGIN PM */

/* USER CODE END PM */

/* Private variables ---------------------------------------------------------*/
/* USER CODE BEGIN Variables */

extern uint8_t g_uart_rx_byte;
extern volatile float current_speed;
extern UART_HandleTypeDef huart1;

/* USER CODE END Variables */
/* Definitions for CommTask */
osThreadId_t CommTaskHandle;
const osThreadAttr_t CommTask_attributes = {
  .name = "CommTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for ControlTask */
osThreadId_t ControlTaskHandle;
const osThreadAttr_t ControlTask_attributes = {
  .name = "ControlTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for DisplayTask */
osThreadId_t DisplayTaskHandle;
const osThreadAttr_t DisplayTask_attributes = {
  .name = "DisplayTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for FlowDetectTask */
osThreadId_t FlowDetectTaskHandle;
const osThreadAttr_t FlowDetectTask_attributes = {
  .name = "FlowDetectTask",
  .stack_size = 128 * 4,
  .priority = (osPriority_t) osPriorityNormal,
};
/* Definitions for LuxQueue */
osMessageQueueId_t LuxQueueHandle;
const osMessageQueueAttr_t LuxQueue_attributes = {
  .name = "LuxQueue"
};
/* Definitions for CmdQueue */
osMessageQueueId_t CmdQueueHandle;
const osMessageQueueAttr_t CmdQueue_attributes = {
  .name = "CmdQueue"
};

/* Private function prototypes -----------------------------------------------*/
/* USER CODE BEGIN FunctionPrototypes */

// static int parse_uint(const char *s, int *value);
void SendDataToESP(char *data);
void ParseCommand(char *command);
void SendHeartbeat(void);
void SendStatus(void);

/* USER CODE END FunctionPrototypes */

void StartCommTask(void *argument);
void StartControlTask(void *argument);
void StartDisplayTask(void *argument);
void StartFlowDetectTask(void *argument);

void MX_FREERTOS_Init(void); /* (MISRA C 2004 rule 8.1) */

/**
  * @brief  FreeRTOS initialization
  * @param  None
  * @retval None
  */
void MX_FREERTOS_Init(void) {
  /* USER CODE BEGIN Init */
  
  // HAL_GPIO_TogglePin(light_GPIO_Port, light_Pin);
  // HAL_Delay(500);
  // HAL_GPIO_TogglePin(light_GPIO_Port, light_Pin);
  // HAL_Delay(500);
  // HAL_GPIO_TogglePin(light_GPIO_Port, light_Pin);
  // HAL_Delay(500);
  // HAL_GPIO_TogglePin(light_GPIO_Port, light_Pin);

	BH1750_Init();
  BH1750_Start(0x10);
	SSD1315_Init();

  // HAL_GPIO_TogglePin(light_GPIO_Port, light_Pin);
  // HAL_Delay(500);
  // HAL_GPIO_TogglePin(light_GPIO_Port, light_Pin);
  // HAL_Delay(500);
  // HAL_GPIO_TogglePin(light_GPIO_Port, light_Pin);
  // HAL_Delay(500);
  // HAL_GPIO_TogglePin(light_GPIO_Port, light_Pin);

  SSD1315_ShowString(0, 0, "System Init");
  SSD1315_ShowString(0, 2, "WiFi Connecting");
  SSD1315_ShowString(0, 4, "......");
  SSD1315_Update();
  System_Handshake(&huart1);
  SSD1315_Clear();
  SSD1315_ShowString(0, 0, "System Init");
  SSD1315_ShowString(0, 2, "WiFi Connected");
  SSD1315_Update();
  HAL_Delay(1000);
  /* USER CODE END Init */

  /* USER CODE BEGIN RTOS_MUTEX */
  /* add mutexes, ... */
  /* USER CODE END RTOS_MUTEX */

  /* USER CODE BEGIN RTOS_SEMAPHORES */
  /* add semaphores, ... */
  /* USER CODE END RTOS_SEMAPHORES */

  /* USER CODE BEGIN RTOS_TIMERS */
  /* start timers, add new ones, ... */
  /* USER CODE END RTOS_TIMERS */

  /* Create the queue(s) */
  /* creation of LuxQueue */
  LuxQueueHandle = osMessageQueueNew (2, sizeof(uint16_t), &LuxQueue_attributes);

  /* creation of CmdQueue */
  CmdQueueHandle = osMessageQueueNew (4, 16, &CmdQueue_attributes);

  /* USER CODE BEGIN RTOS_QUEUES */
  /* add queues, ... */
  /* USER CODE END RTOS_QUEUES */

  /* Create the thread(s) */
  /* creation of CommTask */
  CommTaskHandle = osThreadNew(StartCommTask, NULL, &CommTask_attributes);

  /* creation of ControlTask */
  ControlTaskHandle = osThreadNew(StartControlTask, NULL, &ControlTask_attributes);

  /* creation of DisplayTask */
  DisplayTaskHandle = osThreadNew(StartDisplayTask, NULL, &DisplayTask_attributes);

  /* creation of FlowDetectTask */
  FlowDetectTaskHandle = osThreadNew(StartFlowDetectTask, NULL, &FlowDetectTask_attributes);

  /* USER CODE BEGIN RTOS_THREADS */
  /* add threads, ... */
  /* USER CODE END RTOS_THREADS */

  /* USER CODE BEGIN RTOS_EVENTS */
  /* add events, ... */
  /* USER CODE END RTOS_EVENTS */

}

/* USER CODE BEGIN Header_StartCommTask */
/**
  * @brief  Function implementing the CommTask thread.
  * @param  argument: Not used
  * @retval None
  */
/* USER CODE END Header_StartCommTask */
void StartCommTask(void *argument)
{
  /* USER CODE BEGIN StartCommTask */
  TickType_t xLastWakeTime;
  const TickType_t xFrequency = pdMS_TO_TICKS(REPORT_INTERVAL);
  xLastWakeTime = xTaskGetTickCount();

  TickType_t xLastCmdTime = 0;  // 用于检查超时
  const TickType_t timeout = pdMS_TO_TICKS(UART_TIMEOUT);  // 超时8秒

  /* Infinite loop */
  for(;;)
  {
    char received_data[100];
    int len = HAL_UART_Receive(&huart1, (uint8_t *)received_data, sizeof(received_data), 100);
    if (len > 0) {
      ParseCommand(received_data);
      xLastCmdTime = xTaskGetTickCount();  // 更新命令解析时间
    }

    // 检查是否超时
    if (xTaskGetTickCount() - xLastCmdTime > timeout) {
      // 超过8秒，拉低EspRst端口，重启ESP8266
      HAL_GPIO_WritePin(EspRst_GPIO_Port, EspRst_Pin, GPIO_PIN_RESET); // 拉低复位端口
      osDelay(100);  // 延迟足够时间让ESP重启
      HAL_GPIO_WritePin(EspRst_GPIO_Port, EspRst_Pin, GPIO_PIN_SET); // 释放复位端口
      xLastCmdTime = xTaskGetTickCount(); // 重置计时
    }

    if (alarm_state != ALARM_NONE) {
      SendStatus();
      xLastWakeTime = xTaskGetTickCount();
    }

    if (xTaskGetTickCount() - xLastWakeTime >= xFrequency) {
      SendStatus();
      xLastWakeTime = xTaskGetTickCount();
    }

    osDelay(100);
  }
  /* USER CODE END StartCommTask */
}

/* USER CODE BEGIN Header_StartControlTask */
/**
* @brief Function implementing the ControlTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartControlTask */
void StartControlTask(void *argument)
{
  /* USER CODE BEGIN StartControlTask */
  // const TickType_t delay_time = pdMS_TO_TICKS(100);

  /* Infinite loop */
  for(;;)
  {
    osDelay(100);
    if (system_state == WORKING)
    {
      switch(alarm_state)
      {
        case ALARM_COMPLETE:
          HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
          break;

        case ALARM_FAST:
          HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
          break;

        case ALARM_LOW:
          HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_SET);
          break;

        case ALARM_NONE:
        default:
          HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);
          break;
      }
    }
    else // system_state == IDLE
    {
      HAL_GPIO_WritePin(Buzzer_GPIO_Port, Buzzer_Pin, GPIO_PIN_RESET);
    }
  }

  /* USER CODE END StartControlTask */
}

/* USER CODE BEGIN Header_StartDisplayTask */
/**
* @brief Function implementing the DisplayTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartDisplayTask */
void StartDisplayTask(void *argument)
{
  /* USER CODE BEGIN StartDisplayTask */
  /* Infinite loop */
  for(;;)
  {
    SSD1315_Clear();
    uint16_t luxValue = BH1750_Read();
    char buffer[20];
    // {
    //   float lux = luxValue / 1.2; // Convert raw value to lux
    //   sprintf((char*)buffer, "Raw: %u", luxValue);
    //   snprintf((char*)buffer, sizeof(buffer), "Light: %.2f", lux);
    //   SSD1315_ShowString(0, 6, (char*)buffer);
    // }
    {
      snprintf((char*)buffer, sizeof(buffer), "Raw:   %d", luxValue);
      SSD1315_ShowString(0, 6, (char*)buffer);
    }
    snprintf((char*)buffer, sizeof(buffer), "State: %s", Get_State_String(system_state));
    SSD1315_ShowString(0, 0, (char*)buffer);
    snprintf((char*)buffer, sizeof(buffer), "Speed: %.2f", current_speed);
    SSD1315_ShowString(0, 4, (char*)buffer);
    snprintf((char*)buffer, sizeof(buffer), "Alarm: %s", Get_Alarm_String(alarm_state));
    SSD1315_ShowString(0, 2, (char*)buffer);
    SSD1315_Update();

    HAL_GPIO_TogglePin(light_GPIO_Port, light_Pin);
    osDelay(500);
  }
  /* USER CODE END StartDisplayTask */
}

/* USER CODE BEGIN Header_StartFlowDetectTask */
/**
* @brief Function implementing the FlowDetectTask thread.
* @param argument: Not used
* @retval None
*/
/* USER CODE END Header_StartFlowDetectTask */
void StartFlowDetectTask(void *argument)
{
  /* USER CODE BEGIN StartFlowDetectTask */

  uint32_t last_speed_tick = xTaskGetTickCount();
  uint32_t last_drop_tick  = xTaskGetTickCount();

  uint16_t sec_count[3] = {0};
  uint8_t index = 0;

  uint32_t sum = 0;

  const uint32_t speed_period = 1000;
  const uint32_t timeout = 5000;

  /* Infinite loop */
  for(;;)
  {
    osDelay(10);

    if (system_state == IDLE)
    {
      drop_count = 0;
      sum = 0;
      for (int i = 0; i < 3; i++) sec_count[i] = 0;
      alarm_state = ALARM_NONE;

      continue;
    }

    if (xTaskGetTickCount() - last_speed_tick >= speed_period)
    {
      uint16_t new_count;

      taskENTER_CRITICAL();
      new_count = drop_count;
      drop_count = 0;
      taskEXIT_CRITICAL();

      if (new_count > 0)
      {
        last_drop_tick = xTaskGetTickCount();
      }

      sum -= sec_count[index];
      sec_count[index] = new_count;
      sum += sec_count[index];

      index++;
      if (index >= 3) index = 0;

      current_speed = (float)sum / 3.0f;

      last_speed_tick = xTaskGetTickCount();
    }

    if (xTaskGetTickCount() - last_drop_tick >= timeout)
    {
      alarm_state = ALARM_COMPLETE;
    }
    else
    {
      if (current_speed > 5.0f)
      {
        alarm_state = ALARM_FAST;
      }
      else if (current_speed < 0.5f && current_speed > 0.0f)
      {
        alarm_state = ALARM_LOW;
      }
      else
      {
        alarm_state = ALARM_NONE;
      }
    }

  }

  /* USER CODE END StartFlowDetectTask */
}

/* Private application code --------------------------------------------------*/
/* USER CODE BEGIN Application */

/** replace from atoi(), but more fast **
 ** UNUSED **
static int parse_uint(const char *s, int *value) {
  int num = 0;
  if (!s || *s < '0' || *s > '9') return 0;
  while (*s >= '0' && *s <= '9') {
    num = num * 10 + (*s++ - '0');
  }
  if (*s != '\0' && *s != '\r' && *s != '\n') return 0;
  *value = num;
  return 1;
}
*/

void SendDataToESP(char *data) {
  HAL_UART_Transmit(&huart1, (uint8_t *)data, strlen(data), 100);
}

void ParseCommand(char *command) {
  if (!command || !command[0]) return;

  char first = command[0];
  if (first == 'H') {   // Heartbeat
    if (command[1] == 'B' && (command[2] == '\0' || command[2] == '\r' || command[2] == '\n')) {
      SendDataToESP("ACK\n");
      goto clear;
    }
  }
  else if (first == 'C') {   // Command
    if (strncmp(command, "CMD,", 4) != 0) {
      goto clear;
    }

    char *cmd = command + 4;
    if (strncmp(cmd, "START", 5) == 0) {
      char n = cmd[5];
      if (n == '\0' || n == '\r' || n == '\n') {
        system_state = WORKING;
        SendDataToESP("ACK\n");
        goto clear;
      }
    }
    else if (strncmp(cmd, "STOP", 4) == 0) {
      char n = cmd[4];
      if (n == '\0' || n == '\r' || n == '\n') {
        system_state = IDLE;
        SendDataToESP("ACK\n");
        goto clear;
      }
    }
    else {
      SendDataToESP("ERROR\n");
      goto clear;
    }
  }

clear:
  command[0] = '\0';
}

void SendStatus(void) {
  char status_message[20];

  if (system_state == IDLE) {   // Only report state when idle, to reduce unnecessary updates
    snprintf(status_message, sizeof(status_message), "STATE,%s\n", Get_State_String(system_state) );
    SendDataToESP(status_message);
    return;
  }
  
  snprintf(status_message, sizeof(status_message), "STATE,%s\n", Get_State_String(system_state) );
  SendDataToESP(status_message);

  snprintf(status_message, sizeof(status_message), "SPEED,%.2f\n", current_speed );
  SendDataToESP(status_message);

  if (alarm_state != ALARM_NONE) {   // Only report alarm when it's active, to reduce unnecessary updates
    snprintf(status_message, sizeof(status_message), "ALARM,%s\n", Get_Alarm_String(alarm_state) );
    SendDataToESP(status_message);
  }
}

/* USER CODE END Application */

