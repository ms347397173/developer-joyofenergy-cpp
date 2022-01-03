#ifndef DEVELOPER_JOYOFENERGY_CPP_BEAST_GENERATOR_H
#define DEVELOPER_JOYOFENERGY_CPP_BEAST_GENERATOR_H

#include <domain/ElectricityReading.h>

#include <vector>

//生成number个随机的电表读数，相邻读数间隔3分钟，但电表读数总是4000
class generator {
 public:
  std::vector<ElectricityReading> generate(int number) {
    std::vector<ElectricityReading> readings;
    readings.reserve(number);
    auto now = std::chrono::system_clock::now();
    for (int i = number; i > 0; i--) {
      auto r = std::abs(std::rand()) % 4000;    //未使用？
      readings.emplace_back(now - std::chrono::minutes(i * 3), 4000);
    }
    return readings;
  }
};

#endif  // DEVELOPER_JOYOFENERGY_CPP_BEAST_GENERATOR_H
