/**
 * host_program_node_processor.h 
 * 
*/


#pragma once


#include "node_processor.h"
#include "host_program.h"


namespace archway {


  class HostProgramNodeProcessor : public NodeProcessor<HostProgramNodeProcessor> {

    public:

      HostProgramNodeProcessor() {}

      virtual int ProcessNode( 
        const YAML::Node& node,
        std::shared_ptr<Archway> archway,
        std::any in_context = nullptr
      ) override;

      
      static std::string YamlNodeName() {
        return "hostProgram";
      }


  };
}