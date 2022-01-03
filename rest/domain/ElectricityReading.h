#ifndef DEVELOPER_JOYOFENERGY_CPP_BEAST_ELECTRICITYREADING_H
#define DEVELOPER_JOYOFENERGY_CPP_BEAST_ELECTRICITYREADING_H

#include <chrono>
#include <ctime>

//时间和读数的简单类，包含一个时间点和一个读数
class ElectricityReading {
 public:
  using time_point_type = std::chrono::time_point<std::chrono::system_clock>;

  ElectricityReading(time_point_type time, size_t reading) : time(time), reading(reading) {}

  time_point_type getTime() const { return time; }

  size_t getReading() const { return reading; }

  bool operator==(const ElectricityReading& rhs) const { return time == rhs.time && reading == rhs.reading; }
  bool operator!=(const ElectricityReading& rhs) const { return !(rhs == *this); }

 private:
  time_point_type time;
  size_t reading; // scale out in 0.1w for precision
};

#endif  // DEVELOPER_JOYOFENERGY_CPP_BEAST_ELECTRICITYREADING_H
