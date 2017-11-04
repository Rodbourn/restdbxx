//
// Created by victor on 4/11/17.
//

#include "UserRequestHandlerFactory.h"
#include "UserRequestHandler.h"
#include "LoggingFilter.h"
#include "Validations.h"
#include <boost/algorithm/string.hpp>
#include <proxygen/httpserver/filters/DirectResponseHandler.h>
namespace restdbxx {

const std::string USERS_PATH = "/__users";
const std::string LALA = "";
void UserRequestHandlerFactory::onServerStart(folly::EventBase *evb) noexcept {

  VLOG(google::GLOG_INFO) << "starting server ";
}
void UserRequestHandlerFactory::onServerStop() noexcept {

  VLOG(google::GLOG_INFO) << "stopping server ";
}
proxygen::RequestHandler *UserRequestHandlerFactory::onRequest(proxygen::RequestHandler *handler,
                                                               proxygen::HTTPMessage *message) noexcept {
  std::string path = message->getPath();
  Validations::sanitize_path(path);
  bool result = boost::algorithm::starts_with(path, USERS_PATH);
  if (result) {
    VLOG(google::GLOG_INFO) << "path matches, handling this request with UserRequestHandler";
    auto filter = new LoggingFilter(new UserRequestHandler());
    return filter;
  } else if (handler == nullptr) {
    auto filter = new LoggingFilter(new proxygen::DirectResponseHandler(404, "Not Found", "whatevah"));

    //return new proxygen::DirectResponseHandler(404,  "not found", "");
  }

  VLOG(google::GLOG_INFO) << "path doesnt match";
  return new LoggingFilter(handler);
}
UserRequestHandlerFactory::~UserRequestHandlerFactory() {

}
UserRequestHandlerFactory::UserRequestHandlerFactory() {}

}