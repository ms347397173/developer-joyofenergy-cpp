#ifndef DEVELOPER_JOYOFENERGY_CPP_BEAST_METERREADINGCONTROLLER_H
#define DEVELOPER_JOYOFENERGY_CPP_BEAST_METERREADINGCONTROLLER_H

#include <service/ElectricityReadingService.h>
#include <service/MeterReadingService.h>

#include <ctime>
#include <iomanip>
#include <iostream>
#include <string>

#include <boost/beast/http.hpp>
#include <nlohmann/json.hpp>

namespace http = boost::beast::http;

namespace detail {
auto toRfc3339(std::chrono::time_point<std::chrono::system_clock> time) {
  std::stringstream ss;
  const auto ctime = std::chrono::system_clock::to_time_t(time);
  ss << std::put_time(std::gmtime(&ctime), "%FT%T");
  return ss.str();
}

auto fromRfc3339(const std::string &time) {
  std::tm tm = {};
  std::stringstream ss(time);
  ss >> std::get_time(&tm, "%Y-%m-%dT%H:%M:%S");
  auto tp = std::chrono::system_clock::from_time_t(std::mktime(&tm));
  return tp;
}

auto renderReadingAsJson(const ElectricityReading &r) {
  return nlohmann::json{{"time", toRfc3339(r.getTime())}, {"reading", double(r.getReading()) / 10000}};
}
}  // namespace detail

//http api 表读写，通过http调用来读表或者存表
class MeterReadingController {
 public:
  MeterReadingController(ElectricityReadingService &electricityReadingService, MeterReadingService &meterReadingService)
      : electricityReadingService(electricityReadingService), meterReadingService(meterReadingService) {}

  http::response<http::string_body> Read(const http::request<http::string_body> &req, const std::vector<std::string> &queries) {
    const auto &meterId = queries[0];
    auto readings = electricityReadingService.GetReading(meterId);

    if (!readings) {
      return {http::status::not_found, req.version()};
    }
    http::response<http::string_body> res{http::status::ok, req.version()};
    res.set(http::field::content_type, "application/json");
    res.keep_alive(req.keep_alive());
    auto results = nlohmann::json::array();
    std::transform(readings->begin(), readings->end(), std::back_inserter(results), &detail::renderReadingAsJson);  //每个readings中的调用renderReadingAsJson，然后插入到result中
    nlohmann::json j;
    j["readings"] = results;
    res.body() = j.dump();
    res.prepare_payload();
    return res;
  }

  http::response<http::string_body> Store(const http::request<http::string_body> &req,
                                          const std::vector<std::string> &queries) {
    auto body = nlohmann::json::parse(req.body());
    auto smartMeterId = body["smartMeterId"];
    std::vector<ElectricityReading> electricityReadings;
    if (!IsMeterReadingsValid(smartMeterId, body["electricityReadings"])) {
      return {http::status::internal_server_error, 11};
    }
    for (auto &electricityReading : body["electricityReadings"]) {
      electricityReadings.emplace_back(detail::fromRfc3339(electricityReading["time"]), electricityReading["reading"]);
    }
    meterReadingService.storeReadings(smartMeterId, electricityReadings);
    return {};
  }

 private:
  ElectricityReadingService &electricityReadingService;
  MeterReadingService &meterReadingService;
  bool IsMeterReadingsValid(const nlohmann::basic_json<> &smartMeterId,
                            const nlohmann::basic_json<> &electricityReadings) {
    if (smartMeterId.type() == nlohmann::json::value_t::null || electricityReadings.empty()) {
      return false;
    }
    return true;
  }
};

#endif  // DEVELOPER_JOYOFENERGY_CPP_BEAST_METERREADINGCONTROLLER_H
