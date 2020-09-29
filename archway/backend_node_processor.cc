/**
 * backend_node_processor.cc
 * 
*/


#include "backend_node_processor.h"
#include "single_host_backend.h"



using namespace archway;

BackendNodeProcessor::BackendNodeProcessor() {

  _register_backend_constructor(
    "single", 
    [](const std::string& in_name, const YAML::Node& in_node) {
      return new SingleHostBackend(in_name, in_node);
    }
  );

}


int BackendNodeProcessor::ProcessNode( 
  const YAML::Node& in_node,
  const Archway* archway
) {

  int the_result = -1;
  do {

    auto the_kind = in_node["kind"];
    if(! the_kind ) break;

    auto the_name = in_node["name"];
    if( ! the_name) break;

    auto the_backend = _new_backend(
      the_kind.as<std::string>(), 
      the_name.as<std::string>(), 
      in_node
    );

    // Now Save the newly constructed Backend object into the Archway storage.
    // archway->SaveBackend(the_backend);

    the_result = 0;

  } while( false );
  
  
  

  return the_result;
}