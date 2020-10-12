/**
 * router.cc
 * 
*/



#include "router.h"
#include "utils/string_utils.h"


using namespace archway;


Router::Router() {


}


void Router::AddHostProgram(
  const std::vector<std::string>& in_host_names,
  const std::shared_ptr<HostProgram> in_host_program
) {

  for( auto const& the_host_name: in_host_names) {
          
    // Check for wildcad symbol at the start of `in_host_name`
    //  and if it exists, transform and add it to the special map
    //  for these kind of hosts
    if( the_host_name.front() == '*') {

      auto the_inverted_host_name = StringUtils::InvertHostName( the_host_name);
      host_programs_by_wildcard.emplace( the_inverted_host_name, in_host_program);

    } else {
      
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
  }

  return the_host_program;
}