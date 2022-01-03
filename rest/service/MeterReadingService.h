//
// Created by Cenxi Zhang on 2021/8/18.
//

#ifndef DEVELOPER_JOYOFENERGY_CPP_BEAST_METERREADINGSERVICE_H
#define DEVELOPER_JOYOFENERGY_CPP_BEAST_METERREADINGSERVICE_H

#include <domain/ElectricityReading.h>

#include <iostream>
#include <map>
#include <mutex>
#include <optional>
#include <shared_mutex>
#include <string>
#include <unordered_map>
#include <vector>

//电表读取服务，可以存取电表读数列表
class MeterReadingService {
 public:
  //读取某个电表的读数列表
  std::optional<std::vector<ElectricityReading>> getReadings(const std::string &smartMeterId) {
    std::shared_lock<std::shared_mutex> lock(mtx);
    if (meterAssociatedReadings.find(smartMeterId) == meterAssociatedReadings.end()) {
      return {};
    }
    return meterAssociatedReadings[smartMeterId];
  }

  //存储电表读数列表
  void storeReadings(const std::string &smartMeterId, std::vector<ElectricityReading> &electricityReadings) {
    std::unique_lock<std::shared_mutex> lock(mtx);
    if (meterAssociatedReadings.find(smartMeterId) == meterAssociatedReadings.end()) {
      meterAssociatedReadings[smartMeterId] = {}; //已经存在，置空
    }
    meterAssociatedReadings[smartMeterId].insert(meterAssociatedReadings[smartMeterId].end(), electricityReadings.begin(),
                                                 electricityReadings.end());
  }

  explicit MeterReadingService(std::unordered_map<std::string, std::vector<ElectricityReading>> &meterAssociatedReadings)
      : meterAssociatedReadings(meterAssociatedReadings) {}

 private:
  std::unordered_map<std::string, std::vector<ElectricityReading>> &meterAssociatedReadings;
  mutable std::shared_mutex mtx;
};

#endif  // DEVELOPER_JOYOFENERGY_CPP_BEAST_METERREADINGSERVICE_H
