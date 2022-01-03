#ifndef DEVELOPER_JOYOFENERGY_CPP_BEAST_ENDPOINTTEST_H
#define DEVELOPER_JOYOFENERGY_CPP_BEAST_ENDPOINTTEST_H

#include <gmock/gmock.h>
#include <rest/server.h>

#include "RestClient.h"

//依赖RestClient来发起http请求，包含一个server
class EndpointTest : public ::testing::Test {
 public:
  EndpointTest() { server.run(kHost, kPort); }

 private:
  server server{1};
  constexpr static const char *const kHost{"127.0.0.1"};
  constexpr static unsigned short kPort{8080};

 protected:
  RestClient client{kHost, kPort};
};

#endif  // DEVELOPER_JOYOFENERGY_CPP_BEAST_ENDPOINTTEST_H
