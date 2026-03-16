#ifndef __ISR_H
#define __ISR_H

#include "main.h"
#include "FreeRTOS.h"
#include "task.h"
#include "cmsis_os.h"

// 声明在 freertos.c 中定义的任务句柄
extern osThreadId_t CommTaskHandle; 

// 如果有全局缓冲区，也可以在这里声明
extern uint8_t g_uart_rx_byte;

void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart);
// void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart);

void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin);

#endif
