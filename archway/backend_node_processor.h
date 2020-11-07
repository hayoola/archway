/**
 * backend_node_processor.h
 * 
*/


#pragma once


#include "node_processor.h"
#include "backend.h"


namespace archway {

  

  class BackendNodeProcessor : public NodeProcessor<BackendNodeProcessor> {

    public:

      BackendNodeProcessor();

      virtual int ProcessNode( 
        const YAML::Node& node,
        std::shared_ptr<Archway> archway,
        std::any in_context = nullptr
      ) override;

      static std::string YamlNodeName() {
        return "backend"; 
      }

  };


}