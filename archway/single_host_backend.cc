/**
 * single_host_backend.cc
 * 
*/


#include "single_host_backend.h"


using namespace archway;


SingleHostBackend::SingleHostBackend(
  const std::string& in_name,
  const YAML::Node& node
) : Backend(in_name, node) {


}

std::string SingleHostBackend::ActiveAccessPath() {

  std::string the_path{""};

  return the_path;
}

int SingleHostBackend::HandleEvent(
  const Event& in_event
) {

  return 0;
}