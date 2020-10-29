/**
 * node_processor.h
 * 
 * Base class for node processor objects that are responsible
 *  for processing nodes inside the YAML config file
 * 
**/

#pragma once


#include <unordered_map>
#include <string>
#include <string_view>
#include <functional>
#include <memory>
#include <trantor/utils/Logger.h>
#include "yaml-cpp/yaml.h"
#include "archway.h"


namespace archway {

  
  /**
   * The abstract base class of all archway compiler's NodeProcessors
   * Don't subclass this class, but use the templated NodeProcessor class.
  */
  class NodeProcessorBase {

    public:

      virtual int ProcessNode( 
        const YAML::Node& node,
        Archway* archway,
        std::any in_context = nullptr
      ) = 0;

  };


  class NodeProcessorRegistry {

    public:

      /**
       * @brief Register a 'factory function' for creating a NodeProcessor
       *  that is capable of processing the 'node_name'
       * @param node_name The YAML node name, which this processor can process
       * @param function A factory function 
      **/
      static void registerProcessor(
        const std::string& node_name, 
        const std::function<NodeProcessorBase *()> func
      );


      /**
       * @brief Creates a new instance of the NodeProcessor object
       *  which is registered to process 'node_name'.
       * In conformance to C++ core guideline we won't use the singleton
       *  pattern anymore, because of that the returned instance is 
       *  protected by <shared_ptr> so it will be self-destructed
       * @param node_name The registered YAML node name
       * @return A generic unique pointer of the NodeProcessor object 
       *  (non-constant for enabling the caller to take the ownership)
      **/
      static std::shared_ptr<NodeProcessorBase> NewInstance(
        const std::string &node_name
      );


      static std::unordered_map<
        std::string, 
        std::function<NodeProcessorBase *()>
      > &getMap();

  };

  
  /**
   * @brief Each concrete subclass of NodeProcessor should have a static member
   *  function with this signature: `static const std:string YamlNodeName()`
   * 
  **/
  template<typename T>
  class NodeProcessor : public virtual NodeProcessorBase {

    public:

    protected:

      /**
       * Prevents direct instantiation of this class
      */
      NodeProcessor() {
        
        allocator_.EnableRegistration();
      }


    private:
      
      class Allocator {

        public:

          Allocator() {
            RegisterProcessorClass();
          }

          /**
           * This is a dummy function tp prevents the linker from removing
           *  the references to dynamically allocated NodeProcessors
          */
          void EnableRegistration() {

          }

          // TODO: Define two versions of templeted 'RegisterClass'
          // TODO:  to check for the existence of T::YamlNodeName' static method
          
          void RegisterProcessorClass() {

            NodeProcessorRegistry::registerProcessor( 
              T::YamlNodeName(),
              []() -> NodeProcessorBase* { return new T;}
            );
            

          }

      };

      // This static member causes the registration 
      static Allocator allocator_;
  };

  // Define the the typename of internal allocatr object to make
  //  the linker happy!
  template<typename T>
  typename NodeProcessor<T>::Allocator NodeProcessor<T>::allocator_;
  
}