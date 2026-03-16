#include "state.h"


const char* SystemStateStrings[] = {
  "IDLE",
  "WORKING"
};

const char* AlarmStateStrings[] = {
  "NONE",
  "COMPLETE",
  "LOW",
  "FAST"
};

volatile uint16_t drop_count = 0;          // 中断计数
volatile uint16_t xLastCmdTime = 0;        // 上次收信时间
volatile float current_speed;              // 当前流速
volatile SystemState_t system_state;       // 系统状态
volatile AlarmState_t alarm_state;         // 报警状态
volatile AlarmState_t last_alarm_state;    // 上次报警状态

const char* Get_State_String(SystemState_t state) {
  if (state < STATE_MAX) {
    return SystemStateStrings[state];
  }
  return "UNKNOWN";
}

const char* Get_Alarm_String(AlarmState_t state) {
  if (state < ALARM_MAX) {
    return AlarmStateStrings[state];
  }
  return "UNKNOWN";
}
