/**
 *
 *  dynamic_function.h
 * 
 *  Archway subroutine container
 *
*/

#pragma once

#include <vector>
#include <functional>
#include "archway_types.h"
#include "utils/expected.h"


namespace archway
{
  
  using Instruction = 
    std::function<Expected<void>(Message&)>
  ;  

  /**
   * A sequence of Instructions
  */ 
  class DynamicFunction {

    public:

    DynamicFunction() {

    }

    void AddInstruction( const Instruction in_instruction) {
      instructions_.emplace_back(in_instruction);
    }

    
    Expected<void> Run( Message &);

    private:

    std::vector<Instruction> instructions_;
  };
} // namespace Archway
