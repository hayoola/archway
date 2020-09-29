/**
 * node_processor.cc
 * 
 * 
**/


#include "node_processor.h"
#include <trantor/utils/Logger.h>


using namespace archway;




std::unique_ptr<NodeProcessorBase> NodeProcessorRegistry::NewInstance(
  const std::string& in_node_name
) {

  auto iter = getMap().find(in_node_name);
  if( iter != getMap().end()) {

    return std::unique_ptr<NodeProcessorBase> (iter->second());
  
  } else {

    return nullptr;
  }
}



void NodeProcessorRegistry::registerProcessor(
  const std::string& node_name, 
  const std::function<NodeProcessorBase *()> factory_func
) {

  LOG_DEBUG << "Register NodeProcessor: " << node_name;

  auto the_result_pair = getMap().emplace(node_name, factory_func);
  if( ! the_result_pair.second ) {
    LOG_ERROR << "Can't register the NodeProcessor for: " << node_name;
  }
}




std::unordered_map<
  std::string, 
  std::function<NodeProcessorBase *()>
> &NodeProcessorRegistry::getMap() {

  static std::unordered_map<std::string, std::function<NodeProcessorBase *()>> s_map;
  return s_map;
}