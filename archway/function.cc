/**
 *
 *  function.cc
 *
*/

#include "function.h"


using namespace drogon;
using namespace archway;

Action Function::Run( Message & in_message) {

  Action the_result{Action::kInvalid};

  for( auto the_iter = instructions_.begin(); the_iter != instructions_.end(); the_iter++ ) {
    the_result = (*the_iter)(in_message);
    //TODO: Process the result
  }

  return the_result;

}