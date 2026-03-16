#include "isr.h"
#include "state.h"

/**
  * @brief UART 接收完成回调函数
  * @param huart: UART 句柄
  */
void HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)
{
  // 检查是哪一个串口触发的中断
  if (huart->Instance == USART1) 
  {
    BaseType_t xHigherPriorityTaskWoken = pdFALSE;

    /* 1. 发送任务通知给接收任务 */
    // vTaskNotifyGiveFromISR 是最快的同步方式
    vTaskNotifyGiveFromISR(CommTaskHandle, &xHigherPriorityTaskWoken);

    /* 2. 重新开启中断接收 (非常重要，否则中断只触发一次) */
    // 将收到的数据存入 g_uart_rx_byte，长度为 1
    HAL_UART_Receive_IT(huart, &g_uart_rx_byte, 1);

    /* 3. 强制进行上下文切换 */
    // 如果接收任务优先级高，退出中断后立即执行任务，不等待滴答定时器
    portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
  }
}

/**
  * @brief GPIO 外部中断回调函数
  * @param GPIO_Pin: 触发中断的 GPIO 引脚编号
  */
void HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)
{
  if (GPIO_Pin == PhotoelectricSensor_Pin)
  {
    drop_count++;
  }
}

// /**
//   * @brief UART 发送完成回调函数 (可选)
//   */
// void HAL_UART_TxCpltCallback(UART_HandleTypeDef *huart)
// {
//     if (huart->Instance == USART1)
//     {
//         // 可以在这里处理发送完一帧后的逻辑，比如释放发送信号量
//     }
// }
