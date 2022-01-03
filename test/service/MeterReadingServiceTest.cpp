#include <gmock/gmock.h>
#include <rest/service/MeterReadingService.h>

//简单的stl容器unordered_map放入取出测试
TEST(MeterReadingServiceTest, GetReadingsShouldReturnNullGivenMeterIdThatDoesNotExist) {
  std::unordered_map<std::string, std::vector<ElectricityReading>> meterAssociatedReadings;
  MeterReadingService meterReadingService{meterAssociatedReadings};

  EXPECT_FALSE(meterReadingService.getReadings("unknown-id").has_value());
}

TEST(MeterReadingServiceTest, GetReadingsShouldReturnMeterReadingsGivenMeterReadingThatExists) {
  std::unordered_map<std::string, std::vector<ElectricityReading>> meterAssociatedReadings;
  MeterReadingService meterReadingService{meterAssociatedReadings};
  std::vector<ElectricityReading> emptyReadings;
  meterReadingService.storeReadings("some-id", emptyReadings);

  auto readings = meterReadingService.getReadings("some-id");

  EXPECT_TRUE(readings.has_value());
  EXPECT_TRUE(readings.value().empty());
}
