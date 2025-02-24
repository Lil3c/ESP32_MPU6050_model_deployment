#ifndef MODEL_MANAGER_H
#define MODEL_MANAGER_H

#include <Arduino.h>
#include "tensorflow/lite/micro/micro_mutable_op_resolver.h"
#include "tensorflow/lite/micro/micro_interpreter.h"
#include "tensorflow/lite/micro/system_setup.h"
#include "tensorflow/lite/schema/schema_generated.h"

#define BUFFER_SIZE 50
#define SENSOR_DIMS 6 

// 模型、解释器和张量在ModelManager namespace中声明
namespace ModelManager{
  extern const tflite::Model* model;         // 声明model时const很重要
  extern tflite::MicroInterpreter* interpreter;
  extern TfLiteTensor* model_input;
  extern TfLiteTensor* model_output;

  void initializeModel();
  void runInference(float localBuffer[BUFFER_SIZE][SENSOR_DIMS]);
}

#endif
