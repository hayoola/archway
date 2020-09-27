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
#include <variant>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>
#include "archway_types.h"


namespace archway
{
  
  using Instruction = std::variant<
    std::function<Action(const drogon::HttpRequestPtr &)>,
    std::function<Action(const drogon::HttpResponsePtr &)>
  >;

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
