/**
 * declarations_node_processor.cc
 * 
*/


#include "declarations_node_processor.h"


using namespace archway;


DeclarationsNodeProcessor::DeclarationsNodeProcessor() {


}


int DeclarationsNodeProcessor::ProcessNode( 
  const YAML::Node& in_node,
  std::shared_ptr<Archway> archway,
  std::any in_context
) {

  int the_result{0};

  do {

    if( in_node.Type() != YAML::NodeType::Sequence) {
      the_result = -1;
      break;
    }

    for( const auto& the_child : in_node ) {

      auto the_backend_node = the_child["backend"];
      if(! the_backend_node ) {
        the_result = -2;
        break;
      }

      auto the_processor = NodeProcessorRegistry::NewInstance( "backend");
      if( ! the_processor) {
        the_result = -3;
        break;
      }

      the_result = the_processor->ProcessNode( the_backend_node, archway);
      if( the_result != 0 ) break;
    }

  } while( false );

  return the_result;
}