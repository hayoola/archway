/**
 * single_host_backend_node_processor.cc
 * 
*/




#include "single_host_backend_node_processor.h"
#include "single_host_backend.h"


using namespace archway;


int SingleHostBackendNodeProcessor::ProcessNode( 
  const YAML::Node& in_node,
  std::shared_ptr<Archway> archway,
  std::any in_context
) {

  int the_result{0};

  do {

    auto the_backend_name_node = in_node["name"];
    if( ! the_backend_name_node ) {
      the_result = -1;
      break;
    }

    if( the_backend_name_node.Type() != YAML::NodeType::Scalar ) {
      the_result = -2;
      break;
    }

    std::string the_backend_name = the_backend_name_node.as<std::string>();
    if( the_backend_name.size() < 1 ) {
      the_result = -3;
      break;
    }

    auto the_hosts_node = in_node["hosts"];
    if( ! the_hosts_node ) {
      the_result = -4;
      break;
    }

    if( the_hosts_node.Type() != YAML::NodeType::Sequence ) {
      the_result = -5;
      break;
    }

    auto the_1st_host = the_hosts_node[0];
    if( ! the_1st_host ) {
      the_result = -6;
      break;
    }

    auto the_address_node = the_1st_host["address"];
    if( ! the_address_node ) {
      the_result = -7;
      break;
    }

    if( the_address_node.Type() != YAML::NodeType::Scalar ) {
      the_result = -8;
      break;
    }

    std::string the_address = the_address_node.as<std::string>();
    if( the_address.size() < 4 ) {
      the_result = -9;
      break;
    }

    auto the_backend = std::make_shared<SingleHostBackend>(
      the_backend_name,
      the_address
    );

    archway->AddBackend(the_backend);

  } while( false );

  return the_result;
}