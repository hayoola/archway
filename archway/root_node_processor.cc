/**
 * root_node_processor.cc
 * 
**/


#include "root_node_processor.h"



using namespace archway;

RootNodeProcessor::RootNodeProcessor() 
  : node_names_({"declarations", "routines"}) {

}

int RootNodeProcessor::ProcessNode( 
  const YAML::Node& node,
  Archway* archway,
  std::any in_context
) {
  
  int the_result = 0; // Indicates OK!

  using NodeNameIterator_t = std::vector<std::string>::const_iterator;

  for( NodeNameIterator_t iter = node_names_.begin(); iter != node_names_.end(); ++iter ) {

    YAML::Node the_node = node[ *iter];
    if( ! the_node ) {
      the_result = -1;
      break;
    }

    auto the_processor = NodeProcessorRegistry::NewInstance( *iter);
    if( ! the_processor ) {
      the_result = -2;
      break;
    }

    the_result = the_processor->ProcessNode( the_node, archway);
    if( the_result != 0 ) {
      break;
    }
      
  }

  return the_result;
}