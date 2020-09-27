/**
 *
 *  set_backend_instruction.h
 *
*/

#pragma once

#include <drogon/DrObject.h>
#include <drogon/HttpRequest.h>
#include "archway_types.h"



namespace archway {


  class SetBackendInstruction : public drogon::DrObject<SetBackendInstruction> {

    public:
    
    SetBackendInstruction() {

    }

    Action operator () (const drogon::HttpRequestPtr & request) {
      return processRequest(request);
    }

    Action processRequest(const drogon::HttpRequestPtr &);

  };

}