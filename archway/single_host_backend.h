/**
 * single_host_backend.h
 * 
*/

#pragma once

#include "backend.h"


namespace archway {

  class SingleHostBackend: public Backend {

    public:

      SingleHostBackend(
        const std::string& in_name,
        const YAML::Node& node
      );

      std::string ActiveAccessPath() override;

      int HandleEvent( const Event& in_event) override;

  };

}