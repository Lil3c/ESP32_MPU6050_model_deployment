#include "sensor_manager.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <freertos/semphr.h>

extern SemaphoreHandle_t bufferMutex;    // SemaphoreHandle_t是在freertos中用于表示信号量的句柄类型，信号量是一种同步机制，通常用于在任务之间进行互斥访问或协调资源使用
extern TaskHandle_t inferenceTaskHandle; // TaskHandle_t是在freertos中用于表示任务的句柄类型，一个指向任务控制块的指针

Adafruit_MPU6050 mpu;   // 构建传感器实例

float sensorBuffer[BUFFER_SIZE][SENSOR_DIMS];  // 如果有多个任务会读取或写入sensorbuffer，这种情况下，必须使用信号量或其他同步机制来防止数据竞争
volatile int bufferIndex = 0;

namespace SensorManager{
  void initSensor(){
    Wire.begin(10, 11);    // SDA在前，SCL在后
    if(!mpu.begin()){
      Serial.println("无法找到 MPU6050 传感器！");
      while(1);
    }
    Serial.println("MPU6050 初始化成功。");
  }

  void sensorTask(void *parameter){
    while(1){
      sensors_event_t a, g, temp;       // Adafruit和其他硬件相关库中用于表示传感器数据的结构体
      mpu.getEvent(&a, &g, &temp);

      // 使用互斥锁保护缓冲区写入
      xSemaphoreTake(bufferMutex, portMAX_DELAY);  // freertos中用于获取信号量的函数调用，portmax_delay表示在等待信号量的时候可以无限期阻塞
      sensorBuffer[bufferIndex][0] = a.acceleration.x;
      sensorBuffer[bufferIndex][1] = a.acceleration.y;
      sensorBuffer[bufferIndex][2] = a.acceleration.z;
      sensorBuffer[bufferIndex][3] = g.gyro.x;
      sensorBuffer[bufferIndex][4] = g.gyro.y;
      sensorBuffer[bufferIndex][5] = g.gyro.z;
      bufferIndex++;

      // 当缓冲区达到设定大小时，通知推理任务
      if(bufferIndex >= BUFFER_SIZE && inferenceTaskHandle != NULL){    // 与，检查inferencetaskhandle是否不为空（即已初始化正在运行）
        xTaskNotifyGive(inferenceTaskHandle);      // freertos中用于向特定任务发送通知的函数
      }
      xSemaphoreGive(bufferMutex);                 // freertos中用于释放之前获得的信号量的函数

      // 保持 100Hz 的采样率
      vTaskDelay(pdMS_TO_TICKS(10));         // freertos中延迟函数，该函数使任务在延迟期间不占用cpu资源，允许其他任务运行
    }
  } 
}
