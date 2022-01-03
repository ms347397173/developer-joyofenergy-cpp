#ifndef DEVELOPER_JOYOFENERGY_CPP_BEAST_PRICEPLANSERVICE_H
#define DEVELOPER_JOYOFENERGY_CPP_BEAST_PRICEPLANSERVICE_H

#include <configuration.h>
#include <domain/ElectricityReading.h>
#include <domain/PricePlan.h>
#include <service/MeterReadingService.h>

#include <ctime>
#include <map>
#include <optional>
#include <string>
#include <vector>

//价格计划服务
class PricePlanService {
 public:
  using time_point_type = std::chrono::time_point<std::chrono::system_clock>;

  //获取指定智能电表当前电量对应每个价格计划的花费
  std::optional<std::map<std::string, int>> getConsumptionCostOfElectricityReadingsForEachPricePlan(std::string smartMeterId) {
    std::optional<std::vector<ElectricityReading>> electricityReadings = meterReadingService.getReadings(smartMeterId);
    if (!electricityReadings.has_value()) {
      return {};
    }

    std::map<std::string, int> consumptionCostOfElectricityReadingsForEachPricePlan;
    for (auto pricePlan : pricePlans) {
      consumptionCostOfElectricityReadingsForEachPricePlan.insert(
          std::make_pair(pricePlan.getPlanName(), calculateCost(electricityReadings.value(), pricePlan)));
    }
    return consumptionCostOfElectricityReadingsForEachPricePlan;
  }

  PricePlanService(std::vector<PricePlan> &pricePlans, MeterReadingService &meterReadingService)
      : pricePlans(pricePlans), meterReadingService(meterReadingService) {}

 private:
  const std::vector<PricePlan> &pricePlans;
  MeterReadingService &meterReadingService;

  //获取电量列表的时间长度范围，即[最小时间点, 最大时间点]，单位：秒
  static auto calculateTimeElapsed(std::vector<ElectricityReading> electricityReadings) {
    ElectricityReading first = *electricityReadings.begin();
    ElectricityReading last = *electricityReadings.begin();
    std::vector<ElectricityReading>::iterator it;
    for (it = electricityReadings.begin(); it != electricityReadings.end(); it++) {
      if (it->getTime() < first.getTime()) {
        first = *it;
      }
      if (it->getTime() > first.getTime()) {
        last = *it;
      }
    }

    std::chrono::duration duration = last.getTime() - first.getTime();

    return duration;
  }

  //计算花费，平均电量/时间长度（单位：小时）*费用 
  static int calculateCost(const std::vector<ElectricityReading> &electricityReadings, const PricePlan &pricePlan) {
    int average = calculateAverageReading(electricityReadings);
    auto secondsElapsed = std::chrono::duration_cast<std::chrono::seconds>(calculateTimeElapsed(electricityReadings));

    int averagedCost = average * 3600 / secondsElapsed.count();
    return averagedCost * pricePlan.getUnitRate();
  }

  //算电量列表的所有平均，即电量总和/电量列表数量
  static int calculateAverageReading(const std::vector<ElectricityReading> &electricityReadings) {
    int summedReadings = 0;
    for (auto &electricityReading : electricityReadings) {
      summedReadings += electricityReading.getReading();
    }

    return summedReadings / int(electricityReadings.size());
  }
};

#endif  // DEVELOPER_JOYOFENERGY_CPP_BEAST_PRICEPLANSERVICE_H
