/**
 * archway.cc
*/


#include <sstream>
#include "archway.h"
#include "compiler.h"


using namespace archway;

/**
 * A dummy descented of the Archway class to address
 *  the issue of using a protected constructor in
 *  std::make_shared()
*/
struct ArchwayDecendent : public Archway {

};




std::shared_ptr<Archway> Archway::Create(
  std::function<drogon::HttpClientPtr(const std::string&)> in_http_client_factory
) {
  
  auto the_archway_object = std::make_shared<ArchwayDecendent>();
  the_archway_object->router_ = std::make_shared<Router>(in_http_client_factory);
  the_archway_object->compiler_ = std::make_shared<Compiler>(
    the_archway_object->shared_from_this()
  );
  
  return the_archway_object;
}





Expected<void> Archway::Compile(
  const std::string& in_file_name 
) {

  Expected<void> the_result{};

  if( ! compiler_ ) {
    return CompileError("The Compiler object hasn't been constructed!");
  }

  int the_compile_result = compiler_->CompileFile(in_file_name);

  if( the_compile_result != 0 ) {

    the_result = CompileError("Can't compile the config file! error code: " + the_compile_result);
  }

  return the_result;
}




Expected<void> Archway::Route(
  const drogon::HttpRequestPtr& in_request,
  drogon::AdviceCallback&& in_drogon_advice_callback
) {

  Expected<void> the_result{};

  the_result = router_->Route( in_request, std::move(in_drogon_advice_callback));

  return the_result;
}

