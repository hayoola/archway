/**
 * router.cc
 * 
*/



#include <exception>
#include <drogon/HttpTypes.h>
#include "router.h"
#include "utils/string_utils.h"
#include "utils/expected.h"


using namespace archway;


struct ReceiveRequestHandler {

  Expected<void> operator () ( std::shared_ptr<Message>& in_message) {
    
    Expected<void> the_result{};

    do {

      auto the_host_program = std::any_cast<std::shared_ptr<HostProgram>> (
        &(in_message->parameter(ParamID::kHostProgram))   // Use pointer any_cast to not throw!
      );

      if( ! the_host_program ) {
        the_result = RoutingError("The Message doesn't bear 'host_program' param!");
        break;
      }
      

      // We set the default next stage, but the dynamic function can override
      //?? In the initial version we don't have 'cache', so we just 
      //??  turn to the 'Fetch' stage!
      in_message->parameter(ParamID::kStage) = Stage::kBackendFetch;
      
      
      auto the_dynamic_receive_request_func = (*the_host_program)->
        GetDynamicFunctionForStage(Stage::kReceive)
      ;

      if( the_dynamic_receive_request_func ) {
        the_result = the_dynamic_receive_request_func->Run(in_message);
      }


      // Check to see whether the dynamic function set a backend or not
      auto the_backend_idx_ptr = std::any_cast<int> (
        &(in_message->parameter(ParamID::kBackendIndex))   // Use pointer any_cast to not throw!
      );

      int the_backend_idx = 0;  // The default backend is the first one
      if( the_backend_idx_ptr ) {
        the_backend_idx = *the_backend_idx_ptr;
      
      } else {

        // There is no backend index param inside the message, so we set it here!
        in_message->parameter(ParamID::kBackendIndex) = the_backend_idx;
      }

      


      auto the_router = std::any_cast<std::shared_ptr<Router>> (
        &(in_message->parameter(ParamID::kRouterPtr))
      );
      
      if( ! the_router ) {
        the_result = RoutingError("There is not a pointer to the Router inside in_message!");
        break;
      }

      the_result = (*the_router)->ExecuteNextStage(in_message);


    } while( false );
    
    
    return the_result;
  }

};




struct FetchResponseHandler {

  Expected<void> operator () ( std::shared_ptr<Message>& in_message) {

    Expected<void> the_result{};

    do {

      auto the_host_program = std::any_cast<std::shared_ptr<HostProgram>> (
        in_message->parameter(ParamID::kHostProgram)
      );

      if( ! the_host_program ) {
        the_result = RoutingError("The Message doesn't bear 'host_program' param!");
        break;
      }

      
      auto the_router = std::any_cast<std::shared_ptr<Router>> (
        &(in_message->parameter(ParamID::kRouterPtr))
      );
      
      if( ! the_router ) {
        the_result = RoutingError("There is not a pointer to the Router inside in_message!");
        break;
      }
      
      auto the_backend_idx_ptr = std::any_cast<int> (
        &(in_message->parameter(ParamID::kBackendIndex))   // Use pointer any_cast to not throw!
      );

      if( ! the_backend_idx_ptr ) {
        the_result = RoutingError("No backend was set before!");
        break;
      }

      int the_backend_idx = *the_backend_idx_ptr;
      auto the_backend = (*the_router)->GetBackend(the_backend_idx);
      if( !the_backend ) {
        the_result = RoutingError("The backend pointer is null !");
        break;
      }

      the_result = the_backend->Fetch(
        (*the_router)->HttpClientFactory(),
        in_message,
        [] (std::shared_ptr<Message>& in_response_message) {

          Expected<void> the_callback_result{};

          do {

            auto the_fetch_result_ptr = std::any_cast<drogon::ReqResult> (
              &(in_response_message->parameter(ParamID::kFetchStatus))  // Use pointer any_cast to not throw!
            );

            if( !the_fetch_result_ptr ) {
              the_callback_result = RoutingError("There is no fetch result status in the message !");
              break;
            }

            auto the_fetch_result = *the_fetch_result_ptr;
            if( the_fetch_result == drogon::ReqResult::Ok ) {

              // We know `in_response_message` contains a valid response
              //  So on the next stage we have chance to process this reponse
              in_response_message->parameter(ParamID::kStage) = Stage::kBackendResponse;
            
            } else {

              // On the next stage, we should synthesize an error response 
              in_response_message->parameter(ParamID::kStage) = Stage::kBackendError;
            }

            auto the_router = std::any_cast<std::shared_ptr<Router>> (
              &(in_response_message->parameter(ParamID::kRouterPtr))
            );

            if( ! the_router ) {
              the_callback_result = RoutingError("There is no pointer to router inside the response message !");
              break;
            }

            the_callback_result = (*the_router)->ExecuteNextStage( in_response_message);

          } while (false);

          return the_callback_result;
        }
      );

      // Now we should create a drogon HttpClient and hand over the request
      // These are the questions:
      //  Where should I store the ClientPtr? In a thread-local storage?
      //    -> Yes, of course! We should put them in a `drogon::IOThreadStorage`
      //        data member!
      //  When should I initialize the thread-local storage?
      //    -> Well, the Archway and all of its related objects would be 
      //        initialized during the drogon plugin's `initAndStart` method
      //        so the Router's constructor looks like a proper place for
      //        initializing the `drogon::IOThreadStorage` data members
      //
      // ? Actually, we don't have to create a HttpClient here anymore!
      //  ? Since we handover this burden to the backend, so we just call `Fetch`
      //  ? and wait till the provided callback gets called


    } while( false );

    return the_result;
  }

};



struct BackendResponseHandler {

  Expected<void> operator () ( std::shared_ptr<Message>& in_message) {

    Expected<void> the_result{};

    do {

      auto the_host_program = std::any_cast<std::shared_ptr<HostProgram>> (
        in_message->parameter(ParamID::kHostProgram)
      );

      if( ! the_host_program ) {
        the_result = RoutingError("The Message doesn't bear 'host_program' param!");
        break;
      }

      in_message->parameter(ParamID::kStage) = Stage::kDeliver;

      //TODO: Call the dynamic function

      auto the_router = std::any_cast<std::shared_ptr<Router>> (
        &(in_message->parameter(ParamID::kRouterPtr))
      );

      if( ! the_router ) {
        the_result = RoutingError("There is no pointer to router inside the response message !");
        break;
      }

      the_result = (*the_router)->ExecuteNextStage( in_message);

    } while( false );

    return the_result;
  }

};




struct BackendErrorHandler {

  Expected<void> operator () ( std::shared_ptr<Message>& in_message) {

    Expected<void> the_result{};

    do {

      auto the_host_program = std::any_cast<std::shared_ptr<HostProgram>> (
        in_message->parameter(ParamID::kHostProgram)
      );

      if( ! the_host_program ) {
        the_result = RoutingError("The Message doesn't bear 'host_program' param!");
        break;
      }

    } while( false );

    return the_result;

  }

};




struct DeliverResponseHandler {

  Expected<void> operator () ( std::shared_ptr<Message>& in_message) {

    Expected<void> the_result{};

    do {

      auto the_host_program = std::any_cast<std::shared_ptr<HostProgram>> (
        in_message->parameter(ParamID::kHostProgram)
      );

      if( ! the_host_program ) {
        the_result = RoutingError("The Message doesn't bear 'host_program' param!");
        break;
      }

      // TODO: Find the `DeliverResponse` dynamic function and call it

      // The dynamic function can't change the next stage, because here is the end!
      in_message->parameter(ParamID::kStage) = Stage::kEnd;

      auto the_drogon_advice_callback_ptr = std::any_cast<drogon::AdviceCallback> (
        &(in_message->parameter(ParamID::kDrogonAdviseCallback))  // Use pointer any_cast to not throw!
      );

      if(! the_drogon_advice_callback_ptr ) {
        the_result = RoutingError("There is no Drogon Advice Callback inside the message!");
        break;
      }

      auto the_response = in_message->Response();
      if( !the_response ) {
        the_result = RoutingError("There is no Response inside the message");
        break;
      }

      // It's time to deliver!
      (*the_drogon_advice_callback_ptr)(the_response);


    } while( false );

    return the_result;

  }

};




Router::Router(std::function<drogon::HttpClientPtr(const std::string&)> in_http_client_factory) : 
  http_client_factory_(in_http_client_factory),
  default_host_program_(nullptr) {


  // Initialize `built_in_stage_functions_` with function objects
  //  that are defined in this file
  built_in_stage_functions_.SetFunctionForStage( ReceiveRequestHandler(), Stage::kReceive);
  built_in_stage_functions_.SetFunctionForStage( FetchResponseHandler(), Stage::kBackendFetch);
  built_in_stage_functions_.SetFunctionForStage( BackendResponseHandler(), Stage::kBackendResponse);
  built_in_stage_functions_.SetFunctionForStage( BackendErrorHandler(), Stage::kBackendError);
  built_in_stage_functions_.SetFunctionForStage( DeliverResponseHandler(), Stage::kDeliver);
}




Expected<void> Router::Route(
  const drogon::HttpRequestPtr& in_request,
  drogon::AdviceCallback&& in_drogon_advice_callback
) {

  Expected<void> the_result{};  // A successful Expected<void>

  auto the_host_program{default_host_program_};

  std::string the_host_name = in_request->getHeader("host");
  if( the_host_name != "" ) {

    the_host_program = FindHostProgram( the_host_name);
  }

  if( the_host_program ) {

    auto the_message = std::make_shared<Message>(in_request);
    the_message->parameter(ParamID::kHostProgram) = the_host_program;

    // Store the drogon advice callback, which should be called to deliver the response
    the_message->parameter(ParamID::kDrogonAdviseCallback) = in_drogon_advice_callback;
    
    // We store a pointer to the Router for static functions to call 
    //  its `MoveToNextStage` method
    the_message->parameter(ParamID::kRouterPtr) = shared_from_this();
    
    // Before the call to `MoveToNextStage` we should set the next stage!
    the_message->parameter(ParamID::kStage) = Stage::kReceive;
    the_result = ExecuteNextStage( the_message);
  
  } else {

    // The compiler shouldn't allow a configuration without any host_program
    the_result = RoutingError("There is no default host program!");

  }

  return the_result;

}



Expected<void> Router::ExecuteNextStage(
  std::shared_ptr<Message>& in_message
) {

  Expected<void> the_result{};  // A successful Expected<void>

  do {

    auto the_next_stage = std::any_cast<Stage>(in_message->parameter(ParamID::kStage));
    if( the_next_stage == Stage::kStart ) {
      the_result = RoutingError("We can't be at the starting stage in ExecuteNextStage!");
    }

    if( the_next_stage == Stage::kEnd) {
      break;
    }

    if( the_next_stage < Stage::kStart &&
        the_next_stage > Stage::kEnd
    ) {
      the_result = RoutingError("The next stage is invalid! " + 
        static_cast<int>(the_next_stage)
      );
      break;
    }
    
    auto the_function = built_in_stage_functions_.GetStaticFunctionForStage(
      static_cast<Stage>(the_next_stage)
    );
    
    if(! the_function ) {
      the_result = RoutingError("There is no static function for stage: " + 
        static_cast<int>(the_next_stage)
      );
    }

    the_result = the_function(in_message);  // The function will set the next after the next stage!


  } while( false );

  return the_result;

}



void Router::AddHostProgram(
  const std::vector<std::string>& in_host_names,
  const std::shared_ptr<HostProgram> in_host_program
) {

  for( auto const& the_host_name: in_host_names) {
          
    //TODO: Validate `the_host_name` (The wildcard char can 
    //TODO:   only be present at the start of the name following '.')
    
    // Check for wildcad symbol at the start of `in_host_name`
    //  and if it exists, transform and add it to the special map
    //  for these kind of hosts
    if( the_host_name.front() == '*') {

      auto the_normalized_host_name = StringUtils::NormalizeWildcard( the_host_name);

      auto the_inverted_host_name = StringUtils::InvertHostName( the_normalized_host_name);
      if( host_programs_by_wildcard.size() == 0 && default_host_program_ != nullptr ) {
        default_host_program_ = in_host_program;
      }
      host_programs_by_wildcard.emplace( the_inverted_host_name, in_host_program);

    } else {
      
      if( host_programs_by_exact_names.size() == 0 && default_host_program_ != nullptr ) {
        default_host_program_ = in_host_program;
      }
      host_programs_by_exact_names.emplace(the_host_name, in_host_program);
    }
    
    
  }
}



std::shared_ptr<HostProgram> Router::FindHostProgram(
  const std::string& in_host_name
) {

   std::shared_ptr<HostProgram> the_host_program{nullptr};

  auto the_iter = host_programs_by_exact_names.find(in_host_name);
  if( the_iter != host_programs_by_exact_names.end() ) {
    
    the_host_program = the_iter->second;
  
  
  } else {

    // Look into the wildcard map
    // First wen need to 'invert' in_host_name
    auto the_inverted_host_name = StringUtils::InvertHostName( in_host_name);
    std::string the_found_key{""};

    auto the_lower_iter = host_programs_by_wildcard.lower_bound(the_inverted_host_name);
    std::shared_ptr<HostProgram> the_found_program;
    
    if( the_lower_iter == host_programs_by_wildcard.end()) {
      the_found_key = host_programs_by_wildcard.rbegin()->first;
      the_found_program = host_programs_by_wildcard.rbegin()->second;
    
    } else {
       auto the_temp_iter = the_lower_iter;
      the_temp_iter--;
      the_found_key = the_temp_iter->first;
      the_found_program = the_temp_iter->second;
    }

    // Now check the found_key is indeed a prefix of `the_inverted_host_name`
    if( the_inverted_host_name.rfind( the_found_key, 0) == 0 ) {
      the_host_program = the_found_program;
    }
  }

  return the_host_program;
}