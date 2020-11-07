/**
 * routines_node_processor.h
 * 
*/

#pragma once


#include "node_processor.h"


namespace archway {

  class RoutinesNodeProcessor: public NodeProcessor<RoutinesNodeProcessor> {

    public:

      RoutinesNodeProcessor();

      virtual int ProcessNode( 
        const YAML::Node& node,
        std::shared_ptr<Archway> archway,
        std::any in_context = nullptr
      ) override;

      static std::string YamlNodeName() {
        return "routines";
      }
  };

}