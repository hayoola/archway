/**
 * compiler.cc
 * 
**/


#include "compiler.h"
#include "node_processor.h"
#include "yaml-cpp/yaml.h"


using namespace archway;


Compiler::Compiler( std::shared_ptr<Archway> in_archway) :
  archway_(in_archway) {


}



int Compiler::Compile(const std::string& in_config_content) {

  int the_result = 0;

  try {

    YAML::Node the_root_node = YAML::Load( in_config_content);
    auto the_root_processor = NodeProcessorRegistry::NewInstance("_root_");
    the_result = the_root_processor->ProcessNode(the_root_node, archway_);

  } catch( YAML::ParserException exception) {

    the_result = -1;

  }
  
  

  return the_result;

}