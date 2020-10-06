/**
 * backend_node_processor.h
 * 
*/


#pragma once


#include "node_processor.h"
#include "backend.h"


namespace archway {

  using BackendFactoryFunc = 
    std::function<
      std::shared_ptr<Backend>(const std::string& in_name, const YAML::Node& node)
    >
  ;

  class BackendNodeProcessor : public NodeProcessor<BackendNodeProcessor> {

    public:

      BackendNodeProcessor();

      virtual int ProcessNode( 
        const YAML::Node& node,
        Archway* archway,
        std::any in_context = nullptr
      ) override;

      static std::string YamlNodeName() {
        return "backend"; 
      }


    private:

      void _register_backend_constructor(
        const std::string& in_kind,
        const BackendFactoryFunc& in_backend_factory
      ) {

        registry_.emplace(in_kind, in_backend_factory);
      }

      
      std::shared_ptr<Backend> _new_backend(
        const std::string& in_kind,
        const std::string& in_name,
        const YAML::Node& in_node
      ) {

        auto the_iter = registry_.find(in_kind);
        if( the_iter != registry_.end() ) {

          return the_iter->second(in_name, in_node);

        } else {

          return nullptr;
        }
      }

      std::unordered_map<
        std::string, 
        std::function<
          std::shared_ptr<Backend> (
            const std::string& in_name, 
            const YAML::Node& in_node
          )
        >
      > registry_;
  };


}