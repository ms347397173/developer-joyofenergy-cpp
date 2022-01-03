#ifndef DEVELOPER_JOYOFENERGY_CPP_BEAST_ROUTER_H
#define DEVELOPER_JOYOFENERGY_CPP_BEAST_ROUTER_H

#include <regex>
#include <string>
#include <unordered_map>

#include <boost/beast.hpp>

namespace beast = boost::beast;
namespace http = beast::http;

//location绑定类，用mem_fn绑定成员函数，关联path和具体的处理函数，参见server.cpp router.to
class router {
 public:
  template <class Body = http::string_body>
  auto handler() {
    //handle的最终函数，这个通过寻找location来选择对应的函数，最终指向router.to绑定的函数
    return [&](const http::request<Body> &req) -> http::response<http::string_body> {
      const auto uri = req.target();
      for (auto &[path, handler] : handlers_) {
        std::regex re{path};
        std::cmatch matches;
        if (!std::regex_match(uri.begin(), uri.end(), matches, re)) {
          continue;
        }
        std::vector<std::string> queries(matches.begin() + 1, matches.end());
        return handler(req, queries);
      }
      return make_not_found(req, uri);
    };
  }

  template <typename controller_type, auto controller_method, typename... types>
  auto to(const char *path, types &...injections) {
    handlers_[path] = [&](const http::request<http::string_body> &req, const std::vector<std::string> &queries) {
      return std::mem_fn(controller_method)(controller_type{injections...}, req, queries);  //进行绑定
    };
  }

 private:
  template <class Body, class Allocator>
  auto make_not_found(const http::request<Body, http::basic_fields<Allocator>> &req, beast::string_view target) {
    http::response<http::string_body> res{http::status::not_found, req.version()};
    res.set(http::field::server, BOOST_BEAST_VERSION_STRING);
    res.set(http::field::content_type, "text/html");
    res.keep_alive(req.keep_alive());
    res.body() = "The resource '" + std::string(target) + "' was not found.";
    res.prepare_payload();
    return res;
  };

  using Handler = std::function<http::response<http::string_body>(const http::request<http::string_body> &,
                                                                  const std::vector<std::string> &)>;
  std::unordered_map<std::string, Handler> handlers_;
};

#endif  // DEVELOPER_JOYOFENERGY_CPP_BEAST_ROUTER_H
