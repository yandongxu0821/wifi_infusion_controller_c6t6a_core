#include "isr.h"
#include "state.h"

volatile uint32_t last_key_tick = 0;
volatile uint32_t last_photo_tick = 0;

#define KEY_DEBOUNCE_MS 50
#define PHOTO_DEBOUNCE_MS 10

/**
  * @brief UART 接收完成回调函数
  * @param huart: UART 句柄
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart) {
  // 检查是哪一个串口触发的中断
  if (huart->Instance == USART1) {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* 1. 发送任务通知给接收任务 */
    // vTaskNotifyGiveFromISR 是最快的同步方式
    vTaskNotifyGiveFromISR(CommTaskHandle, &xHigherPriorityTaskWoken);

    /* 2. 重新开启中断接收 (非常重要，否则中断只触发一次) */
    // 将收到的数据存入 xGlobalUartRxByte，长度为 1
    HAL_UART_Receive_IT(huart, &xGlobalUartRxByte, 1);

    /* 3. 强制进行上下文切换 */
    // 如果接收任务优先级高，退出中断后立即执行任务，不等待滴答定时器
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}

/**
  * @brief GPIO 外部中断回调函数
  * @param GPIO_Pin: 触发中断的 GPIO 引脚编号
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin) {
  HAL_GPIO_TogglePin(light_GPIO_Port, light_Pin);
  if (GPIO_Pin == PhotoelectricSensor_Pin) {
    uint32_t now = HAL_GetTick();
    if (now - last_photo_tick >= PHOTO_DEBOUNCE_MS) {
      last_photo_tick = now;
      xDropCount++;
    }
  }
  
  if (GPIO_Pin == PowerKey_Pin) {
    uint32_t now = HAL_GetTick();

    // 消抖时间过滤
    if (now - last_key_tick < KEY_DEBOUNCE_MS)
      return;

    last_key_tick = now;

    // 检测是否真的按下
    if (HAL_GPIO_ReadPin(PowerKey_GPIO_Port, PowerKey_Pin) == GPIO_PIN_RESET) {

      if (xSystemState == IDLE) {
        xSystemState = WORKING;
      } else {
        xSystemState = IDLE;
      }
    }
  }
}
