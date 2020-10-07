/**
 * functions_node_processor.cc
 * 
*/




#include "functions_node_processor.h"
#include "host_program.h"
#include "function.h"


using namespace archway;



int FunctionsNodeProcessor::ProcessNode( 
  const YAML::Node& in_node,
  Archway* archway,
  std::any in_context
) {

  int the_result{0};

  do {

    auto the_host_program_ptr = std::any_cast<std::shared_ptr<HostProgram>>(in_context);
    if( !the_host_program_ptr ) {
      the_result = -1;
      break;
    }

    if( in_node.Type() != YAML::NodeType::Sequence) {
      the_result = -2;
      break;
    }

    for( const auto& the_child : in_node ) {

      auto the_stage_node = the_child["stage"];
      if( !the_stage_node ) {
        the_result = -3;
        break;
      }

      std::string the_stage_name = the_stage_node.as<std::string>();
      if( the_stage_name.size() < 3 ) {
        the_result = -4;
        break;
      }

      Stage the_stage = HostProgram::StringToStage( the_stage_name);
      if( the_stage == Stage::kInvalid ) {
        the_result = -5;
        break;
      }

      
      auto the_instructions_node = the_child["instructions"];
      if( !the_instructions_node || the_instructions_node.Type() != YAML::NodeType::Sequence ) {
        the_result = -6;
        break;
      }

      // Create a function as the encompassing context of Instructions and 
      //  add it to the passed 'HostProgram' (as in_context)
      auto the_function_ptr = std::make_shared<Function>();
      the_host_program_ptr->SetFunctionForStage(the_function_ptr, the_stage);

      // Now Let's iterate over the instructions
      for( const auto& the_single_instruction_node : the_instructions_node ) {

        if( the_single_instruction_node.Type() != YAML::NodeType::Map ) {
          the_result = -7;
          break;
        }
        auto the_instruction_key = the_single_instruction_node.begin()->first;
        if( the_instruction_key.Type() != YAML::NodeType::Scalar ) {
          the_result = -8;
          break;
        }

        std::string the_instruction_name = the_instruction_key.as<std::string>();
        if( the_instruction_name.size() < 1 ) {
          the_result = -9;
          break;
        }

        auto the_processor = NodeProcessorRegistry::NewInstance( the_instruction_name);
        if( ! the_processor ) {
          the_result = -10;
          break;
        }

        the_result = the_processor->ProcessNode(
          the_single_instruction_node,
          archway,
          the_function_ptr  // Pass the newly constructed `Function` as the context
        );

      }

    }


  } while( false );

  return the_result;
}