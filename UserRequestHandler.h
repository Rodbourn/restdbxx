//
// Created by victor on 4/11/17.
//

#ifndef RESTDBXX_USERREQUESTHANDLER_H
#define RESTDBXX_USERREQUESTHANDLER_H
#include <proxygen/httpserver/RequestHandler.h>
#include <boost/spirit/include/classic.hpp>

using proxygen::RequestHandler;
using folly::IOBuf;

namespace restdbxx {
class UserRequestHandler : public RequestHandler {
 public:
  void onRequest(std::unique_ptr<proxygen::HTTPMessage> headers) noexcept override;
  void onBody(std::unique_ptr<folly::IOBuf> body) noexcept override;
  void onEOM() noexcept override;
  void requestComplete() noexcept override;
  void onError(proxygen::ProxygenError err) noexcept override;
  void onUpgrade(proxygen::UpgradeProtocol prot) noexcept override;

  virtual ~UserRequestHandler();
 private :
  std::unique_ptr<IOBuf> _body;
  void notFound();
};

}

#endif //RESTDBXX_USERREQUESTHANDLER_H
