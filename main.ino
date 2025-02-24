#include <Arduino.h>
#include "sensor_manager.h"
#include "model_manager.h"
#include "inference_manager.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

// 全局互斥锁和任务句柄，用于任务间通信
SemaphoreHandle_t bufferMutex;
TaskHandle_t inferenceTaskHandle = NULL;

void setup(){
  Serial.begin(115200);

  // 初始化传感器和模型
  SensorManager::initSensor();
  ModelManager::initializeModel();

  // 创建互斥锁
  bufferMutex = xSemaphoreCreateMutex();

  // 创建传感器采集任务，固定在核心1上运行
  xTaskCreatePinnedToCore(
    SensorManager::sensorTask,          // 任务函数
    "Sensor Task",                      // 任务名称
    2048,                               // 堆栈大小
    NULL,                               // 参数
    1,                                  // 优先级
    NULL,                               // 不需要保存任务句柄
    1                                   // 核心编号
  );

  xTaskCreatePinnedToCore(
    InferenceManager::inferenceTask,    // 任务函数
    "Inference Task",                   // 任务名称
    4096,                               // 堆栈大小
    NULL,                               // 参数
    1,                                  // 优先级
    &inferenceTaskHandle,               // 保存任务句柄（便于通知）
    0                                   // 核心编号
  );
}

void loop(){
  
}
