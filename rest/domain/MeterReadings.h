#ifndef DEVELOPER_JOYOFENERGY_CPP_BEAST_METERREADINGS_H
#define DEVELOPER_JOYOFENERGY_CPP_BEAST_METERREADINGS_H

#include <domain/ElectricityReading.h>

#include <list>
#include <string>

//智能表读数类，包含id和多条读数记录，纯读取类，无写入
class MeterReadings {
 public:
  MeterReadings() {}

  MeterReadings(std::string smartMeterId, std::list<ElectricityReading> electricityReadings)
      : smartMeterId(smartMeterId), electricityReadings(electricityReadings){};

  std::list<ElectricityReading> getElectricityReadings() { return electricityReadings; }

  std::string getSmartMeterId() { return smartMeterId; }

 private:
  std::list<ElectricityReading> electricityReadings;
  std::string smartMeterId;
};

#endif  // DEVELOPER_JOYOFENERGY_CPP_BEAST_METERREADINGS_H
