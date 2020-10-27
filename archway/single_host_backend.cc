/**
 * single_host_backend.cc
 * 
*/


#include "single_host_backend.h"


using namespace archway;


SingleHostBackend::SingleHostBackend(
  const std::string& in_name,
  const std::string& in_host_address
) : Backend(in_name),
    host_address_(in_host_address) {

  //TODO: Remove this!
  int the_dummy{8765};

}

std::string SingleHostBackend::ActiveAccessPath() {

  return host_address_;
}


int SingleHostBackend::HandleEvent(
  const Event& in_event
) {

  return 0;
}



Expected<void> SingleHostBackend::Fetch(
  std::shared_ptr<Message> & in_message,
  std::function<Expected<void>(std::shared_ptr<Message>&)> did_fetch_callback
) {

  return {};
}