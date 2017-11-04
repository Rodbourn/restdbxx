#include <unistd.h>
#include <string>

#include <iostream>
#include <folly/init/Init.h>
#include <gflags/gflags.h>
#include <folly/io/async/EventBaseManager.h>
#include <proxygen/httpserver/HTTPServer.h>
#include "DbManager.h"

#include "RestDbRequestHandlerFactory.h"
#include "UserRequestHandlerFactory.h"

using namespace restdbxx;
using namespace proxygen;

using folly::EventBase;
using folly::EventBaseManager;
using folly::SocketAddress;

using Protocol = HTTPServer::Protocol;
#include <glog/logging.h>

DEFINE_int32(http_port, 11000, "Port to listen on with HTTP protocol");
DEFINE_int32(spdy_port, 11001, "Port to listen on with SPDY protocol");
DEFINE_int32(h2_port, 11002, "Port to listen on with HTTP/2 protocol");
DEFINE_string(ip, "localhost", "IP/Hostname to bind to");
DEFINE_int32(threads, 0, "Number of threads to listen on. Numbers <= 0 "
    "will use the number of cores on this machine.");


using google::GLOG_INFO;

int main(int argc, char ** argv) {
  folly::init(&argc, &argv);
  gflags::ParseCommandLineFlags(&argc, &argv, true);
  google::InstallFailureSignalHandler();

  VLOG(GLOG_INFO) << "Starting restdbxx";
  std::vector<HTTPServer::IPConfig> IPs = {
      {SocketAddress(FLAGS_ip, FLAGS_http_port, true), Protocol::HTTP},
      {SocketAddress(FLAGS_ip, FLAGS_spdy_port, true), Protocol::SPDY},
      {SocketAddress(FLAGS_ip, FLAGS_h2_port, true), Protocol::HTTP2},
  };
  VLOG(GLOG_INFO) << "Starting restdbxx";
  VLOG(GLOG_INFO) << "HTTP PORT: " << FLAGS_http_port;
  VLOG(GLOG_INFO) << "SPDY PORT: " << FLAGS_spdy_port;
  VLOG(GLOG_INFO) << "H2 PORT: " << FLAGS_h2_port;



  if (FLAGS_threads <= 0) {
    FLAGS_threads = sysconf(_SC_NPROCESSORS_ONLN);
    CHECK(FLAGS_threads > 0);
  }

  HTTPServerOptions options;
  options.threads = static_cast<size_t>(FLAGS_threads);
  options.idleTimeout = std::chrono::milliseconds(60000);
  options.shutdownOn = {SIGINT, SIGTERM};
  options.enableContentCompression = false;
  options.handlerFactories = RequestHandlerChain()
      .addThen<UserRequestHandlerFactory>()
      .addThen<RestDbRequestHandlerFactory>()
      .build();
  options.h2cEnabled = true;

  HTTPServer server(std::move(options));
  server.bind(IPs);


  // Start HTTPServer mainloop in a separate thread
  std::thread t([&] () {
    folly::setThreadName("Server main thread");
    server.start();
  });

  t.join();
  return 0;
}