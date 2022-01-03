#ifndef DEVELOPER_JOYOFENERGY_CPP_BEAST_PRICEPLAN_H
#define DEVELOPER_JOYOFENERGY_CPP_BEAST_PRICEPLAN_H

#include <string>
#include <utility>
#include <vector>
#include <chrono>

//价格计划
class PricePlan {
  // todo:
 public:
  using time_point_type = std::chrono::time_point<std::chrono::system_clock>;

  //记录一周每天的价格倍率
  class PeakTimeMultiplier {
   public:
    enum DayOfWeek {
      SUNDAY,
      MONDAY,
      TUESDAY,
      WEDNESDAY,
      THURSDAY,
      FRIDAY,
      SATURDAY
    };

    PeakTimeMultiplier(DayOfWeek dayOfWeek, int multiplier) : dayOfWeek(dayOfWeek), multiplier(multiplier) {}

    DayOfWeek dayOfWeek;
    int multiplier;
  };

  PricePlan(std::string planName, std::string energySupplier, int unitRate, std::vector<PeakTimeMultiplier> peakTimeMultipliers)
      : planName(std::move(planName)), energySupplier(std::move(energySupplier)), unitRate(unitRate), peakTimeMultipliers(std::move(peakTimeMultipliers)) {}

  std::string getEnergySupplier() const { return energySupplier; }

  std::string getPlanName() const { return planName; }

  int getUnitRate() const { return unitRate; }

  //判断是否加倍，符合对应日期乘N倍，否则返回原价unitRate
  int getPrice(time_point_type dateTime) const {
    auto time_t_dateTime = std::chrono::system_clock::to_time_t(dateTime);
    auto t = std::localtime(&time_t_dateTime);
    auto it = std::find_if(peakTimeMultipliers.begin(), peakTimeMultipliers.end(), [=](auto &p) {
      return p.dayOfWeek == t->tm_wday;
    });
    if (it == peakTimeMultipliers.end()) {
      return unitRate;
    }
    return unitRate * it->multiplier;
  }

 private:
  const std::string energySupplier;
  const std::string planName;
  const int unitRate;  // unit price per kWh
  const std::vector<PeakTimeMultiplier> peakTimeMultipliers;
};

#endif  // DEVELOPER_JOYOFENERGY_CPP_BEAST_PRICEPLAN_H
