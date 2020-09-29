/**
 *
 *  function.h
 * 
 *  Archway subroutine container
 *
*/

#pragma once

#include <vector>
#include <functional>
#include "archway_types.h"


namespace archway
{
  
  using Instruction = 
    std::function<Action(const Message &)>
  ;  

  /**
   * A sequence of Instructions
   */ 
  class Function {

    public:

    Function() {

    }

    void AddInstruction( const Instruction& instruction);

    void Run();

    private:

    std::vector<Instruction> instructions_;
  };
} // namespace Archway
