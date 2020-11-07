/**
 * single_host_backend.h
 * 
*/

#pragma once

#include "backend.h"
#include <drogon/IOThreadStorage.h>


namespace archway {

  class SingleHostBackend: public Backend {

    public:

      SingleHostBackend(
        const std::string& in_name,
        const std::string& in_host_address,
        const size_t& in_connection_factor = 5
      );

      std::string ActiveAccessPath() override;

      int HandleEvent( const Event& in_event) override;

      virtual Expected<void> Fetch(
        std::function<drogon::HttpClientPtr(const std::string&)> in_http_client_factory,
        std::shared_ptr<Message> & in_message,
        std::function<Expected<void>(std::shared_ptr<Message>&)> did_fetch_callback
      ) override;

    private:
      const std::string host_address_;
      const size_t connection_factor_;
      drogon::IOThreadStorage<std::vector<drogon::HttpClientPtr>> clients_;
      //std::vector<drogon::HttpClientPtr> clients_;
      drogon::IOThreadStorage<size_t> client_index_{0};
      //size_t client_index_{0};
  };

}