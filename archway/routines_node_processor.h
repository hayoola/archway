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
        const Archway* archway
      ) override;

      static std::string YamlNodeName() {
        return "routines";
      }
  };

}