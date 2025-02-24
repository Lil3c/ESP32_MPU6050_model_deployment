#include "inference_manager.h"
#include "sensor_manager.h"
#include "model_manager.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

// 这里的互斥锁和任务句柄在main.cpp中定义，使用 extern 引用
extern SemaphoreHandle_t bufferMutex;

namespace InferenceManager{
  void inferenceTask(void *parameter){
    for(;;){
      ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

      // 复制数据到局部缓冲区，并重置全局缓冲区索引
      float localBuffer[BUFFER_SIZE][SENSOR_DIMS];
      xSemaphoreTake(bufferMutex, portMAX_DELAY);
      memcpy(localBuffer, sensorBuffer, sizeof(sensorBuffer));   // memcpy 在内存中复制数据
      bufferIndex = 0;    // 通过置0来清空sensorbuffer，让都进来的数据又从头开始存放
      xSemaphoreGive(bufferMutex);

      Serial.println("开始模型推理……");
      unsigned long startTime = millis();
      ModelManager::runInference(localBuffer);
      unsigned long inferenceTime = millis() - startTime;
      Serial.print("Inference Time: ");
      Serial.print(inferenceTime);
      Serial.println(" ms");
    }
  }
}
