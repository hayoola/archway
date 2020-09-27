/**
 * routines_node_processor.h
 * 
*/

#pragma once


#include "node_processor.h"


namespace archway {

  class RoutinesNodeProcessor: public virtual NodeProcessor<RoutinesNodeProcessor> {

    public:

      RoutinesNodeProcessor();

      virtual int ProcessNode( 
        const YAML::Node& node,
        const Archway* archway
      );

      static std::string YamlNodeName() {
        return "routines"; 
      }
  };

}