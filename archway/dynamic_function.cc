/**
 *
 *  function.cc
 *
*/

#include "dynamic_function.h"


using namespace drogon;
using namespace archway;

Expected<void> DynamicFunction::Run( Message & in_message) {

  Expected<void> the_result{};

  for( auto the_iter = instructions_.begin(); the_iter != instructions_.end(); the_iter++ ) {
    the_result = (*the_iter)(in_message);
    //TODO: Process the result
  }

  return the_result;

}