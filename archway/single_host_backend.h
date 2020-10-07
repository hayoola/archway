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
        const std::string& in_host_address
      );

      std::string ActiveAccessPath() override;

      int HandleEvent( const Event& in_event) override;

    private:
      const std::string host_address_;

  };

}