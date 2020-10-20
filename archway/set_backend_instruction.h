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
#include "utils/expected.h"



namespace archway {


  /**
   * An Instruction object.
   * Ironically, all Instruction object are  non-virtual,
   *  which makes them faster to execute.
  */
  class SetBackendInstruction {

    public:
    
      SetBackendInstruction(const int in_backend_index) :
        backend_index_(in_backend_index) {

      }

      Expected<void> operator () (Message& in_message) {
        return processRequest(in_message);
      }

      Expected<void> processRequest(Message &);

    private:
      int backend_index_;

  };

}