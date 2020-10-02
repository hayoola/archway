/**
 *
 *  set_backend_instruction.h
 *
*/

#pragma once

#include <drogon/DrObject.h>
#include <drogon/HttpRequest.h>
#include "archway_types.h"
#include "initable.h"



namespace archway {


  /**
   * An Instruction object.
   * Ironically, all Instruction object are NOT virtual,
   *  which makes them faster to execute.
  */
  class SetBackendInstruction {

    public:
    
      SetBackendInstruction(const int in_backend_index) :
        backend_index_(in_backend_index) {

      }

      Action operator () (Message& in_message) {
        return processRequest(in_message);
      }

      Action processRequest(Message &);

    private:
      int backend_index_;

  };

}