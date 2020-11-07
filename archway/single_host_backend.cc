/**
 * single_host_backend.cc
 * 
*/


#include "single_host_backend.h"
#include <drogon/HttpTypes.h>


using namespace archway;


SingleHostBackend::SingleHostBackend(
  const std::string& in_name,
  const std::string& in_host_address,
  const size_t& in_connection_factor
) : Backend(in_name),
    host_address_(in_host_address),
    connection_factor_(in_connection_factor) {

  
  // The `init` method, registers an 'init callback' function,
  //  which is called the 'numOfThreads' times to initialize the
  //  storage for each thread.
  // On each thread we have a unique vector of clients!
  clients_.init(
        
    [this](
      std::vector<drogon::HttpClientPtr> &clients, 
      size_t ioLoopIndex) {
      
      clients.resize( connection_factor_);
    }
  );
  
  //clients_.resize(connection_factor_);


  // We maintain an index to the clients vector inside the thread
  //  local storage.
  // After using each client, we increment the index up to the vector's size
  //  then we move back to the first one
  // For the sake of randomization, we initialize this index for each thread
  //  differently!
  client_index_.init(
      
    [this](
      size_t &index, 
      size_t ioLoopIndex) { 
      
      index = ioLoopIndex; 
    }
  );
  

}


// !! Deprecated and to be removed soon!
std::string SingleHostBackend::ActiveAccessPath() {

  return host_address_;
}


int SingleHostBackend::HandleEvent(
  const Event& in_event
) {

  return 0;
}



Expected<void> SingleHostBackend::Fetch(
  std::function<drogon::HttpClientPtr(const std::string&)> in_http_client_factory,
  std::shared_ptr<Message>& in_message,
  std::function<Expected<void>(std::shared_ptr<Message>&)> did_fetch_callback
) {

  Expected<void> the_result{};

  do {

    size_t the_index;
    auto &the_clients_vector = *clients_;   // The * operator of IOThreadStorage return the local storage
    //auto &the_clients_vector = clients_;

    the_index = ++(*client_index_) % the_clients_vector.size();
    //the_index = ++client_index_ % the_clients_vector.size();
    auto &the_client_ptr = the_clients_vector[the_index];

    if( !the_client_ptr ) {

      the_client_ptr = in_http_client_factory(host_address_);
      if( !the_client_ptr ) {
        the_result = RoutingError("The http client factory returned null!");
        break;
      }
    }

    auto the_request = in_message->Request();
    if(! the_request ) {
      the_result = RoutingError("There is no request object inside the Message object!");
      break;
    }
    the_request->setPassThrough( true);

    the_client_ptr->sendRequest(
      the_request,
      [did_fetch_callback, &in_message] (
        drogon::ReqResult in_fetch_result,
        const drogon::HttpResponsePtr& in_response
      ) {

        in_message->parameter(ParamID::kFetchStatus) = in_fetch_result;

        auto the_response = in_response;
        if( in_fetch_result != drogon::ReqResult::Ok ) {
          
          // We create a generic error response here, but the user-provided
          //  dynamic functions can put more meaningfull info into the response
          the_response = drogon::HttpResponse::newHttpResponse();
          the_response->setStatusCode(drogon::HttpStatusCode::k500InternalServerError);
        }

        in_message->SetResponse(the_response);
        auto the_result = did_fetch_callback( in_message);
      }
    );

  } while( false );

  return the_result;
}