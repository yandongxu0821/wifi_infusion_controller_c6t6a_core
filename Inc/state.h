#ifndef __STATE_H
#define __STATE_H

#include "main.h"

/* 系统状态枚举 */
typedef enum {
  IDLE = 0,    // 空闲
  WORKING,     // 运行中
  STATE_MAX
} SystemState_t;

/* 报警状态枚举 */
typedef enum {
  ALARM_NONE = 0,
  ALARM_COMPLETE,
  ALARM_LOW,
  ALARM_FAST,
  ALARM_MAX
} AlarmState_t;

extern const char* SystemStateStrings[];
extern const char* AlarmStateStrings[];

extern volatile uint16_t drop_count;
extern volatile uint16_t xLastCmdTime;
extern volatile float current_speed;
extern volatile SystemState_t system_state;
extern volatile AlarmState_t alarm_state;
extern volatile AlarmState_t last_alarm_state;

const char* Get_State_String(SystemState_t state);
const char* Get_Alarm_String(AlarmState_t state);

#endif
