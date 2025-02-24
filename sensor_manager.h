#ifndef SENSOR_MANAGER_H
#define SENSOR_MANAGER_H

#include <Arduino.h>
#include <Adafruit_MPU6050.h>
#include <Adafruit_Sensor.h>
#include <Wire.h>

// 定义采样缓冲区大小和传感器数据维度 
#define BUFFER_SIZE 50            // define特性：定义全局常量（整个文件内可见），该常量没有类型，
#define SENSOR_DIMS 6              // 常量定义在头文件中，其他文件可以通过include该头文件使用该常量

// 声明全局缓冲区变量（后续需要在其他模块中访问）
extern float sensorBuffer[BUFFER_SIZE][SENSOR_DIMS];    // extern关键字用于声明一个变量或函数的外部链接性，即告诉编译器该变量或函数在其他文件中定义，可以不包含头文件，但尽量包含
extern volatile int bufferIndex;                        // volatile关键字告诉编译器某个变量的值可能在程序执行过程中被意外改变，常用于多线程编程

namespace SensorManager {              // namespace用于避免名称冲突，将变量，函数等分组，使用示例 SensorManager::initSensor();
  void initSensor();
  void sensorTask(void *parameter);    // 函数接收一个指向void的指针做为参数，这种类型的指针可以指向任何类型的数据
}

#endif
