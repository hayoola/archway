/**
 * initable.h
 * 
 * Since Instruction objects are dynamically constructed via DrObject, which
 *  has a restriction to use default constructiable classes, this class
 *  comes to rescue.
 * By making Instructions a subclass of this class (beside DrObject) and
 *  implementing the InitSelf method, Compiler gain a chance to initialize 
 *  the instruction by calling the Init method of this class.
 * 
 * TODO: This class is being removed!
 *  
*/

#pragma once

#include "yaml-cpp/yaml.h"
#include "archway.h"


namespace archway {

  class Initable {

    public:

      Initable() :
        is_initialized_(false) {

      }

      void Init( 
        const YAML::Node& in_node,
        const Archway* in_archway
      ) {
        
        if( InitSelf(in_node, in_archway) ) {
          is_initialized_ = true;
        }
      }

      bool IsInitialized() {
        return is_initialized_;
      }

    protected:
      virtual bool InitSelf( 
        const YAML::Node& in_node,
        const Archway* in_archway
      ) = 0;

    private:
      bool is_initialized_;
  };
}