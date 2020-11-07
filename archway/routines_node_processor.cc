/**
 * routines_node_processor.cc
*/


#include "routines_node_processor.h"
#include <string>


using namespace archway;


RoutinesNodeProcessor::RoutinesNodeProcessor() {


}


int RoutinesNodeProcessor::ProcessNode( 
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
      
      auto the_host_program_node = the_child["hostProgram"];
      if( !the_host_program_node ) {
        the_result = -2;
        break;
      }

      /*
      for( const auto& the_program_node: the_host_program_node ) {
        auto the_key = the_program_node.first;
        if( the_key ) {
          std::string the_nm = the_key.as<std::string>();
          int the_dummy{767};
        }
      }
      */

      auto the_processor = NodeProcessorRegistry::NewInstance( "hostProgram");
      if( ! the_processor) {
        the_result = -3;
        break;
      }

      the_result = the_processor->ProcessNode( the_host_program_node, archway);
      if( the_result != 0 ) break;

    }


  } while( false );
  
  

  return the_result;
}