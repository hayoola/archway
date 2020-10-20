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

  Expected<void> operator () ( Message& in_message) {
    
    Expected<void> the_result{};
    
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

    Message the_message(in_request);
    the_message.host_program() = the_host_program;
    the_result = MoveToNextState( the_message);
  
  } {

    // The compiler shouldn't allow a configuration without any host_program
    the_result = std::runtime_error("There is no default host program!");

  }

  return the_result;

}



Expected<void> Router::MoveToNextState(
  Message& in_message
) {

  // auto the_function = build_in_stage_functions_.GetStaticFunctionForStage(
  //  in_message.IntParam(ParamID::kStage));
  // Maybe we need a dedicated Message::stage() accessor function
  //  in_message.stage() = the_default_next_stage;
  //  int the_function();

  return {};

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