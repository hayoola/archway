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
#include <drogon/HttpRequest.h>
#include "archway_types.h"


namespace archway
{
  
  /**
   * A sequnce of Instructions
   */ 
  class Function {

    public:

    Function() {

    }

    void Run();

    private:

    std::vector<std::function<Action(const drogon::HttpRequestPtr &)>> instructions_;
  };
} // namespace Archway
