/**
 *  set_backend_node_processor.cc
 * 
*/




#include "set_backend_node_processor.h"
#include "set_backend_instruction.h"
#include "function.h"


using namespace archway;



int SetBackendNodeProcessor::ProcessNode( 
  const YAML::Node& in_node,
  Archway* archway,
  std::any in_context) {
  
  int the_result = 0;

  do {

    
    auto the_function_ptr = std::any_cast<std::shared_ptr<Function>>(in_context);
    if( ! the_function_ptr ) {
      the_result = -1;
      break;
    }
    
    
    auto the_set_backend_node = in_node["setBackend"];
    if( ! the_set_backend_node ) {
      the_result = -2;
      break;
    }

    if( the_set_backend_node.Type() != YAML::NodeType::Scalar ) {
      the_result = -3;
      break;
    }

    std::string the_backend_name = the_set_backend_node.as<std::string>();
    int the_backend_index = archway->GetBackendIndex(the_backend_name);
    if( the_backend_index == -1 ) {
      the_result = -3;
      break;
    }

    Instruction the_set_backend_instruction = SetBackendInstruction( the_backend_index);
    the_function_ptr->AddInstruction(the_set_backend_instruction);


  } while( false );

  return the_result;

}