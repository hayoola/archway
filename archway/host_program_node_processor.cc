/**
 * host_program_node_processor.cc
 * 
*/




#include "host_program_node_processor.h"
#include "functions_node_processor.h"
#include <memory>


using namespace archway;


int HostProgramNodeProcessor::ProcessNode( 
  const YAML::Node& in_node,
  std::shared_ptr<Archway> archway,
  std::any in_context
) {

  int the_result = -1;
  do {

    auto the_hosts_node = in_node["hosts"];
    if( ! the_hosts_node ) break;

    std::vector<std::string> the_hosts_vector = the_hosts_node.as<std::vector<std::string>>();
    if( the_hosts_vector.size() < 0 ) {
      the_result = -2;
      break;
    }

    // Now we construct a HostProgram (though incomplete) and
    //  store it into the Archway object
    std::shared_ptr<HostProgram> the_host_program_ptr = std::make_shared<HostProgram>();
    archway->AddHostProgram(the_hosts_vector, the_host_program_ptr);
    
    auto the_functions_node = in_node["functions"];
    if( ! the_functions_node ) break;

    auto the_processor = NodeProcessorRegistry::NewInstance( "functions");
    if( ! the_processor ) {
      the_result = -3;
      break;
    }

    the_result = the_processor->ProcessNode(
      the_functions_node,
      archway,
      the_host_program_ptr
    );


  } while( false );


  return the_result;
}