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
      //return new SingleHostBackend(in_name, in_node);
      return std::make_shared<SingleHostBackend>(in_name, in_node);
    }
  );

}


int BackendNodeProcessor::ProcessNode( 
  const YAML::Node& in_node,
  Archway* archway,
  std::any in_context
) {

  int the_result{0};
  do {

    auto the_kind = in_node["kind"];
    if( ! the_kind ) {
      the_result = -1;
      break;
    }

    if( the_kind.Type() != YAML::NodeType::Scalar ) {
      the_result = -2;
      break;
    }

    auto the_name = in_node["name"];
    if( ! the_name) {
      the_result = -3;
      break;
    }

    if( the_name.Type() != YAML::NodeType::Scalar ) {
      the_result = -2;
      break;
    }

    auto the_backend = _new_backend(
      the_kind.as<std::string>(), 
      the_name.as<std::string>(), 
      in_node
    );

    if( ! the_backend ) {
      the_result = -3;
      break;
    }

    // Now Save the newly constructed Backend object into the Archway storage.
    // Check if the_backend == nulptr
    archway->AddBackend(the_backend);

    

  } while( false );
  
  
  

  return the_result;
}