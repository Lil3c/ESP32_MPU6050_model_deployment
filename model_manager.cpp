#include "model_manager.h"
#include "mpu_6050_model.h"

#define BUFFER_SIZE 50
#define SENSOR_DIMS 6

namespace{        
  // 匿名命名空间，在当前文件中可见该变量，其他文件中不可见
  constexpr int kTensorArenaSize = 32 * 1024;
  uint8_t tensor_arena[kTensorArenaSize];
}

namespace ModelManager{
  const tflite::Model* model = nullptr;
  tflite::MicroInterpreter* interpreter = nullptr;
  TfLiteTensor* model_input = nullptr;
  TfLiteTensor* model_output = nullptr;

  void initializeModel(){
    model = tflite::GetModel(mpu_6050_model);
    if (model->version() != TFLITE_SCHEMA_VERSION){
      Serial.println("模型版本不兼容");
      return;
    }

    // 创建op注册器，并添加所需操作
    static tflite::MicroMutableOpResolver<6> resolver;  // op注册
    resolver.AddConv2D();
    resolver.AddMaxPool2D();
    resolver.AddFullyConnected();
    resolver.AddReshape();
    resolver.AddSoftmax();

    // 创建解释器实例
    static tflite::MicroInterpreter static_interpreter(            // static 全局静态变量，该文件中可见，其他文件无法访问
      model, resolver, tensor_arena, kTensorArenaSize, nullptr     // 最后这个空指针位置时error_reporter
    );
    interpreter = &static_interpreter;

    if (interpreter->AllocateTensors() != kTfLiteOk){       // kTFLiteOk 是 TensorFlow Lite 中的一个常量，通常用来表示操作成功的状态
      Serial.println("内存分配失败");                        // TensorFlow Lite 中用于分配张量内存的函数,实现自动分配，包括参数张量，输入，中间，输出等张量
      return;
    }

    model_input = interpreter->input(0);
    model_output = interpreter->output(0); 
  }

  void runInference(float localBuffer[BUFFER_SIZE][SENSOR_DIMS]){
    // 将采集的传感器数据复制到输入张量
    for(int i =0; i < BUFFER_SIZE; i++){
      for(int j = 0; j < SENSOR_DIMS; j++){
        model_input->data.f[i * SENSOR_DIMS + j] = localBuffer[i][j];
      }
    }

    // 执行推理
    if(interpreter->Invoke() != kTfLiteOk){
      Serial.println("推理执行失败");
      return;
    }

    // 解析输出结果
    for(int i = 0; i < model_output->dims->data[1]; i++){
      float output_prob = model_output->data.f[i];
      Serial.print("  Class");
      Serial.print(i);
      Serial.print(": ");
      Serial.print(output_prob, 5);
    }
    Serial.println();
  }
}
