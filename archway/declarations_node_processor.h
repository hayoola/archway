/**
 * declarations_node_processor.h
 * 
 * Processor of the 'declarations' node of the configuration language
*/

#pragma once

#include "node_processor.h"


namespace archway {

  class DeclarationsNodeProcessor : public virtual NodeProcessor<DeclarationsNodeProcessor> {

    public:

      DeclarationsNodeProcessor();

      virtual int ProcessNode( 
        const YAML::Node& node,
        std::shared_ptr<Archway> archway,
        std::any in_context = nullptr
      ) override;

      static std::string YamlNodeName() {
        return "declarations"; 
      }

  };
}