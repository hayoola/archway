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

      virtual Expected<void> Fetch(
        std::shared_ptr<Message> & in_message,
        std::function<Expected<void>(std::shared_ptr<Message>&)> did_fetch_callback
      ) override;

    private:
      const std::string host_address_;

  };

}