/**
 * archway.cc
*/


#include <sstream>
#include "archway.h"


using namespace archway;



Archway::Archway( std::function<drogon::HttpClientPtr(const std::string&)> in_http_client_factory ) {

  router_ = std::make_shared<Router>(in_http_client_factory);
  

}



