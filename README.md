# Core 目录代码分析

本项目是一个基于STM32F103C6的WiFi输液控制器，使用FreeRTOS进行多任务管理，实现与ESP8266的通信、OLED显示、光传感器读取和流量检测等功能。以下是对`Core/Inc`和`Core/Src`目录下所有文件的详细分析。

## Inc 目录

### bh1750.h
**作用**: BH1750环境光传感器驱动的头文件，提供传感器初始化、测量启动和数据读取的函数原型。

**关键函数**:
- `BH1750_Init(void)`: 初始化BH1750传感器。
- `BH1750_Start(uint8_t mode)`: 以指定模式启动测量。
- `BH1750_Read(void)`: 从传感器读取光强度值。

**关键变量**: 无。

**重要说明**: 包含`i2c.h`用于I2C通信。传感器使用I2C地址0x46。

### dma.h
**作用**: DMA（直接内存访问）配置的头文件，由STM32CubeMX生成，包含用户自定义部分。

**关键函数**:
- `MX_DMA_Init(void)`: 初始化DMA控制器并启用中断。

**关键变量**: 无。

**重要说明**: 启用DMA1时钟，并为DMA1_Channel6设置中断。用户部分可用于自定义包含或定义。

### font16x8.h
**作用**: 包含16x8像素字体数组的头文件，定义从空格(0x20)到波浪线(0x7E)的ASCII字符。

**关键函数**: 无。

**关键变量**:
- `Font16x8[][16]`: 95个字符位图数组，每个16字节（8x16像素）。

**重要说明**: 用于SSD1315 OLED驱动的文本渲染。每个字符存储为16字节，上半部分和下半部分用于8x16显示。

### FreeRTOSConfig.h
**作用**: FreeRTOS内核的配置文件，定义系统参数、任务优先级和功能启用。

**关键函数**: 无。

**关键变量**: 无。

**重要说明**: 为STM32F1设置FreeRTOS，56个优先级，3072字节堆，1000Hz滴答率。启用互斥锁、信号量、定时器。用户代码部分的自定义断言处理程序在失败时禁用中断。

### gpio.h
**作用**: GPIO（通用输入输出）配置的头文件，由STM32CubeMX生成，包含用户自定义部分。

**关键函数**:
- `MX_GPIO_Init(void)`: 初始化应用程序的GPIO引脚。

**关键变量**: 无。

**重要说明**: 用户部分允许自定义包含或定义。配置引脚如light、EspRst、Buzzer和PhotoelectricSensor。

### handshake.h
**作用**: UART握手协议的头文件，定义等待特定字符串和执行系统握手的函数。

**关键函数**:
- `UART_WaitForString(UART_HandleTypeDef *huart, const char *target, uint32_t timeout_ms)`: 在UART上等待目标字符串，带超时。
- `System_Handshake(UART_HandleTypeDef *huart)`: 执行三向握手（SYN, ACK,SYN, ACK）。

**关键变量**: 无。

**重要说明**: 使用`main.h`、`isr.h`、`usart.h`。实现ESP8266通信初始化的简单协议。

### i2c.h
**作用**: I2C（集成电路间通信）配置的头文件，由STM32CubeMX生成，包含用户自定义部分。

**关键函数**:
- `MX_I2C1_Init(void)`: 初始化I2C1外设。

**关键变量**:
- `hi2c1`: I2C1实例的I2C_HandleTypeDef。

**重要说明**: 配置I2C1为100kHz，7位寻址。用户部分用于自定义包含或定义。

### isr.h
**作用**: 中断服务例程（ISR）的头文件，声明UART和GPIO中断的回调函数和全局变量。

**关键函数**:
- `HAL_UART_RxCpltCallback(UART_HandleTypeDef *huart)`: UART接收完成回调。
- `HAL_GPIO_EXTI_Callback(uint16_t GPIO_Pin)`: GPIO外部中断回调。

**关键变量**:
- `CommTaskHandle`: 通信任务的句柄。
- `xGlobalUartRxByte`: UART接收的全局字节。

**重要说明**: 包含FreeRTOS头文件用于任务通知。用于处理UART通信和光电传感器中断。

### main.h
**作用**: 应用程序的主头文件，包含HAL头文件并定义引脚配置。

**关键函数**:
- `Error_Handler(void)`: 通过闪烁LED并停止来处理错误。

**关键变量**: 无。

**重要说明**: 定义GPIO引脚：light (PC13)、EspRst (PA1)、Buzzer (PB0)、PhotoelectricSensor (PA15)。包含STM32F1xx HAL。

### ssd1315.h
**作用**: SSD1315 OLED显示驱动的头文件，提供显示控制和文本渲染的函数原型。

**关键函数**:
- `OLED_WriteCmd(uint8_t cmd)`: 向OLED发送命令。
- `SSD1315_Init(void)`: 初始化SSD1315显示。
- `SSD1315_Clear(void)`: 清除显示缓冲区。
- `SSD1315_Update(void)`: 用缓冲区内容更新显示。
- `SSD1315_DrawPoint(uint8_t x, uint8_t y, uint8_t color)`: 在(x,y)绘制点。
- `SSD1315_ShowChar(uint8_t x, uint8_t y, char chr)`: 在位置(x,y)显示字符。
- `SSD1315_ShowString(uint8_t x, uint8_t y, char *str)`: 从(x,y)开始显示字符串。

**关键变量**: 无。

**重要说明**: 包含`i2c.h`用于通信。使用I2C地址0x78。支持128x64分辨率。

### state.h
**作用**: 系统状态管理的头文件，定义系统和报警状态的枚举，以及监控的全局变量。

**关键函数**:
- `Get_State_String(SystemState_t state)`: 返回系统状态的字符串表示。
- `Get_Alarm_String(AlarmState_t state)`: 返回报警状态的字符串表示。

**关键变量**:
- `SystemStateStrings[]`、`AlarmStateStrings[]`: 状态名称的字符串数组。
- `xDropCount`: 滴数的易失计数器（基于中断）。
- `xLastCmdTime`: 最后命令时间。
- `xCurrentSpeed`: 当前流速（浮点数）。
- `xSystemState`: 当前系统状态（IDLE/WORKING）。
- `xAlarmState`: 当前报警状态（NONE/COMPLETE/LOW/FAST）。
- `xLastAlarmState`: 之前的报警状态。

**重要说明**: 枚举定义状态：系统IDLE=0, WORKING=1；报警NONE=0, COMPLETE=1, LOW=2, FAST=3。所有全局变量为易失性以确保线程安全。

### stm32f1xx_hal_conf.h
**作用**: HAL配置文件，由STM32CubeMX生成，启用HAL模块并设置系统参数。

**关键变量/调用**: 启用GPIO、I2C、UART、DMA、TIM等。设置HSE=8MHz，VDD=3.3V，滴答优先级=15。

**重要说明**: 无自定义修改；标准CubeMX输出。

### stm32f1xx_it.h
**作用**: 中断处理程序头文件，由STM32CubeMX生成，声明ISR原型。

**关键函数**: NMI、HardFault等的原型，以及DMA1_Channel6、TIM3、I2C1、USART1等外设ISR。

**重要说明**: 无自定义修改；标准CubeMX输出。

### usart.h
**作用**: USART（通用同步/异步接收器/发送器）配置的头文件，由STM32CubeMX生成，包含用户自定义部分。

**关键函数**:
- `MX_USART1_UART_Init(void)`: 初始化USART1。

**关键变量**:
- `huart1`: USART1的UART_HandleTypeDef。

**重要说明**: 配置USART1为115200波特率，8N1。用户部分用于自定义包含或定义。

## Src 目录

### bh1750.c
**作用**: BH1750光传感器的驱动实现，处理初始化、测量和通过I2C的数据读取。

**关键函数**:
- `BH1750_Init(void)`: 开启电源并重置传感器。
- `BH1750_Start(uint8_t mode)`: 发送测量命令并等待180ms。
- `BH1750_Read(void)`: 从传感器读取2字节并组合为uint16_t。

**关键变量**: `BH1750_ADDR = 0x46`。

**重要说明**: 使用HAL_I2C函数。测量模式0x10（高分辨率）。原始值需要转换为勒克斯（除以1.2）。

### dma.c
**作用**: DMA初始化，由STM32CubeMX生成。

**关键变量/调用**: 启用DMA1时钟，为DMA1_Channel6 IRQ设置优先级5。

**重要说明**: 无自定义修改；标准CubeMX输出。

### freertos.c
**作用**: FreeRTOS应用程序代码，定义任务、队列和通信、控制、显示、流量检测的系统逻辑。

**关键函数**:
- `MX_FREERTOS_Init(void)`: 初始化FreeRTOS，创建任务/队列，执行握手和OLED初始化。
- `StartCommTask`: 处理UART通信，解析命令，发送状态，超时重置ESP。
- `StartControlTask`: 根据系统和报警状态控制蜂鸣器。
- `StartDisplayTask`: 用状态、速度、报警和光数据更新OLED。
- `StartFlowDetectTask`: 从滴数计算流速，设置报警。
- `SendDataToESP`、`ParseCommand`、`SendHeartbeat`、`SendStatus`: 通信助手函数。

**关键变量**:
- 任务: CommTaskHandle, ControlTaskHandle, DisplayTaskHandle, FlowDetectTaskHandle。
- 队列: LuxQueueHandle, CmdQueueHandle。
- 常量: REPORT_INTERVAL=15000ms, UART_TIMEOUT=10000ms。
- 全局: xGlobalUartRxByte, xCurrentSpeed, huart1。

**重要说明**: 使用4个正常优先级任务。CommTask在10s无命令时重置ESP。流量检测使用3秒平均。命令：HB（心跳）、CMD、START/STOP。

### gpio.c
**作用**: GPIO初始化，由STM32CubeMX生成。

**关键变量/调用**: 配置引脚：light（输出）、EspRst（输出上拉）、Buzzer（输出下拉）、PhotoelectricSensor（输入）。

**重要说明**: 无自定义修改；标准CubeMX输出。

### handshake.c
**作用**: UART握手协议的实现。

**关键函数**:
- `UART_WaitForString`: 缓冲传入数据，带超时检查目标字符串。
- `System_Handshake`: 等待SYN，发送ACK,SYN，等待ACK。

**关键变量**: 无。

**重要说明**: 使用32字节缓冲区，用memmove处理字符串匹配以提高效率。握手后刷新UART寄存器。

### i2c.c
**作用**: I2C初始化，由STM32CubeMX生成。

**关键变量/调用**: 配置I2C1：100kHz，7位，无拉伸。MSP：GPIO PB6/7 AF_OD上拉，DMA TX在通道6上。

**重要说明**: 无自定义修改；标准CubeMX输出。

### isr.c
**作用**: UART和GPIO的中断回调实现。

**关键函数**:
- `HAL_UART_RxCpltCallback`: 对于USART1，通知CommTask并重新启动接收。
- `HAL_GPIO_EXTI_Callback`: 在PhotoelectricSensor中断时切换light引脚，增加xDropCount。

**关键变量**: 使用xGlobalUartRxByte, CommTaskHandle。

**重要说明**: UART ISR使用任务通知以提高效率。GPIO ISR处理传感器输入。

### main.c
**作用**: 应用程序主入口，初始化系统、外设并启动FreeRTOS。

**关键函数**:
- `main(void)`: 初始化HAL、时钟、外设、FreeRTOS。
- `SystemClock_Config(void)`: 从HSE设置PLL到48MHz。
- `HAL_TIM_PeriodElapsedCallback`: 为TIM3增加滴答。
- `Error_Handler`: 错误时闪烁LED。

**关键变量**: xGlobalUartRxByte。

**重要说明**: 使用HSE 8MHz，PLL x6 = 48MHz。TIM3用于systick。UART接收在main中启动。

### ssd1315.c
**作用**: SSD1315 OLED显示驱动的实现。

**关键函数**:
- `OLED_WriteCmd`: 通过I2C发送命令。
- `SSD1315_Init`: 用命令初始化显示，包括寻址、对比度等。
- `SSD1315_Clear`: 清零缓冲区。
- `SSD1315_Update`: 通过DMA I2C发送缓冲区。
- `SSD1315_DrawPoint`: 在缓冲区中设置/清除像素。
- `SSD1315_ShowChar`: 使用字体渲染8x16字符。
- `SSD1315_ShowString`: 通过字符渲染字符串。

**关键变量**: `OLED_ADDR = 0x78`，`OLED_GRAM[1025]`（带控制字节的缓冲区）。

**重要说明**: 水平寻址模式以提高DMA效率。字体为16x8。缓冲区为1+1024字节。

### state.c
**作用**: 状态管理的实现。

**关键函数**:
- `Get_State_String`: 返回"IDLE"/"WORKING"/"UNKNOWN"。
- `Get_Alarm_String`: 返回"NONE"/"COMPLETE"/"LOW"/"FAST"/"UNKNOWN"。

**关键变量**: 如上字符串数组。

**重要说明**: 简单的枚举到字符串查找函数。

### stm32f1xx_hal_msp.c
**作用**: MSP初始化，由STM32CubeMX生成。

**关键变量/调用**: 启用AFIO、PWR。设置PendSV优先级15。禁用JTAG。

**重要说明**: 无自定义修改；标准CubeMX输出。

### stm32f1xx_hal_timebase_tim.c
**作用**: 使用TIM3的时间基准，由STM32CubeMX生成。

**关键变量/调用**: 初始化TIM3为1MHz，周期1000用于1ms滴答。处理暂停/恢复。

**重要说明**: 无自定义修改；标准CubeMX输出。

### stm32f1xx_it.c
**作用**: 中断处理程序，由STM32CubeMX生成。

**关键变量/调用**: 故障、DMA、TIM3、I2C、UART的标准处理程序。

**重要说明**: 无自定义修改；标准CubeMX输出。

### system_stm32f1xx.c
**作用**: 系统初始化，由STM32CubeMX生成。

**关键变量/调用**: SystemInit设置向量表。SystemCoreClockUpdate计算时钟。

**重要说明**: 无自定义修改；标准CubeMX输出。

### usart.c
**作用**: USART初始化，由STM32CubeMX生成。

**关键变量/调用**: 配置USART1：115200，8N1。MSP：GPIO PA9/10，中断优先级5。

**重要说明**: 无自定义修改；标准CubeMX输出。