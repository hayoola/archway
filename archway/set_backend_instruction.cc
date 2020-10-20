/**
 *
 *  set_backend_instruction.cc
 *
*/

#include "set_backend_instruction.h"


using namespace drogon;
using namespace archway;






/**
 * This method put the backend_index_ into the Message,
 *  so the Router knows what to do
*/
Expected<void> SetBackendInstruction::processRequest(
  Message& in_message) {

  LOG_DEBUG << "SetBackend Instruction...";
  in_message.SetParam(ParamID::kBackendIndex, backend_index_);

  return {};

}


