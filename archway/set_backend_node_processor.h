/**
 * set_backend_node_processor.h
 * 
*/


#pragma once


#include "node_processor.h"



namespace archway {

  class SetBackendNodeProcessor : public NodeProcessor<SetBackendNodeProcessor> {

    public:

      SetBackendNodeProcessor() {}

      virtual int ProcessNode( 
        const YAML::Node& node,
        Archway* archway,
        std::any in_context = nullptr
      ) override;


      
      static std::string YamlNodeName() {
        return "setBackend";
      }

  };
}