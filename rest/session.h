#ifndef DEVELOPER_JOYOFENERGY_CPP_BEAST_SESSION_H
#define DEVELOPER_JOYOFENERGY_CPP_BEAST_SESSION_H

#include <type_traits>

#include <boost/asio.hpp>
#include <boost/asio/dispatch.hpp>
#include <boost/beast/core/flat_buffer.hpp>
#include <boost/beast/core/tcp_stream.hpp>

#include "logger.h"

//代表一个http会话
//读取到的数据转到预先设置的handler回调_函数中，在listener.h中设置
class session : public std::enable_shared_from_this<session> {
  using error_code = boost::beast::error_code;
  using tcp = boost::asio::ip::tcp;
  using RestHandler = std::function<http::response<http::string_body>(const http::request<http::string_body> &)>;

 public:
  explicit session(tcp::socket &&socket, RestHandler &handler) : stream_(std::move(socket)), handler_(handler) {}

  void run() {
    // We need to be executing within a strand to perform async operations
    // on the I/O objects in this session. Although not strictly necessary
    // for single-threaded contexts, this example code is written to be
    // thread-safe by default.
    boost::asio::dispatch(stream_.get_executor(), boost::beast::bind_front_handler(&session::do_read, shared_from_this()));
  }

 private:
  void do_read() {
    // Make the request empty before MeterReadingController,
    // otherwise the operation behavior is undefined.
    req_ = {};
    stream_.expires_after(std::chrono::seconds(30));
    http::async_read(stream_, buffer_, req_, boost::beast::bind_front_handler(&session::on_read, shared_from_this()));  //回调
  }

  void on_read(error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    // This means they closed the connection
    if (ec == http::error::end_of_stream) {
      return do_close();
    }

    if (ec) {
      return fail(ec, "read");
    }

    send(std::move(handler_(req_)));  //执行
  }

  void on_write(bool close, error_code ec, std::size_t bytes_transferred) {
    boost::ignore_unused(bytes_transferred);

    if (ec) {
      return fail(ec, "write");
    }

    if (close) {
      return do_close();
    }

    res_ = nullptr;
    do_read();
  }

  void do_close() {
    error_code ec;
    stream_.socket().shutdown(tcp::socket::shutdown_send, ec);
  }

  template <class Body, class Fields>
  void send(http::response<Body, Fields> &&msg) {
    auto sp = std::make_shared<typename std::remove_reference<decltype(msg)>::type>(std::move(msg));
    res_ = sp;
    http::async_write(stream_, *sp, boost::beast::bind_front_handler(&session::on_write, shared_from_this(), sp->need_eof()));
  }

  boost::beast::tcp_stream stream_;
  boost::beast::flat_buffer buffer_;
  http::request<http::string_body> req_;
  std::shared_ptr<void> res_;
  RestHandler &handler_;
};

#endif  // DEVELOPER_JOYOFENERGY_CPP_BEAST_SESSION_H
