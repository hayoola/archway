/**
 * root_node_processor.h
 * 
 * The node processor of the YAML root node.
 * It may hand over the processing based on the parsed doc version
 *  to another implementation objects
*/


#pragma once

#include "node_processor.h"
#include <trantor/utils/Logger.h>


namespace archway {

  class RootNodeProcessor : public NodeProcessor<RootNodeProcessor> {

    public:
      RootNodeProcessor();

      virtual int ProcessNode( 
        const YAML::Node& node,
        const Archway* archway
      );

      static std::string YamlNodeName() {
        return "_root_";  // A special node name indicating root (our convention!)
      }

    private:
      const std::vector<std::string> node_names_;

  };

}