/**
 * router.cc
 * 
*/



#include <exception>
#include "router.h"
#include "utils/string_utils.h"
#include "utils/expected.h"


using namespace archway;


struct ReceiveRequestHandler {

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
      
      auto the_dynamic_receive_request_func = the_host_program->
        GetDynamicFunctionForStage(Stage::kReceive)
      ;

      // We set the default next stage, but the dynamic function can override
      //?? In the initial version we don't have 'cache', so we just 
      //??  turn to the 'Fetch' stage!
      in_message->parameter(ParamID::kStage) = Stage::kBackendFetch;
      
      if( the_dynamic_receive_request_func ) {
        the_result = the_dynamic_receive_request_func->Run(in_message);
      }


      auto the_router = std::any_cast<std::shared_ptr<Router>> (
        in_message->parameter(ParamID::kRouterPtr)
      );
      
      if( ! the_router ) {
        the_result = RoutingError("There is not a pointer to the Router inside in_message!");
        break;
      }

      the_result = the_router->MoveToNextState(in_message);


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

    } while( false );

    return the_result;
  }

};




Router::Router() : 
default_host_program_(nullptr) {

  // Initialize `build_in_stage_functions_` with function objects
  //  that are defined in this file
  build_in_stage_functions_.SetFunctionForStage( ReceiveRequestHandler(), Stage::kReceive);

}




Expected<void> Router::Route(
  const drogon::HttpRequestPtr& in_request
) {

  Expected<void> the_result{};  // A successful Expected<void>

  auto the_host_program{default_host_program_};

  std::string the_host_name = in_request->getHeader("host");
  if( the_host_name != "" ) {

    the_host_program = FindHostProgram( the_host_name);
  }

  if( the_host_program ) {

    //Message the_message(in_request);
    auto the_message = std::make_shared<Message>(in_request);
    the_message->parameter(ParamID::kHostProgram) = the_host_program;
    
    // We store a pointer to the Router for static functions to call 
    //  its `MoveToNextStage` method
    the_message->parameter(ParamID::kRouterPtr) = shared_from_this();
    
    // Before the call to `MoveToNextStage` we should set the next stage!
    the_message->parameter(ParamID::kStage) = Stage::kReceive;
    the_result = MoveToNextState( the_message);
  
  } {

    // The compiler shouldn't allow a configuration without any host_program
    the_result = RoutingError("There is no default host program!");

  }

  return the_result;

}



Expected<void> Router::MoveToNextState(
  std::shared_ptr<Message>& in_message
) {

  Expected<void> the_result{};  // A successful Expected<void>

  do {

    auto the_next_stage = std::any_cast<Stage>(in_message->parameter(ParamID::kStage));
    if( the_next_stage == Stage::kStart ) {
      the_result = RoutingError("We can't be at the starting stage in MoveToNextStage!");
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
    
    auto the_function = build_in_stage_functions_.GetStaticFunctionForStage(
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