/**
 * backend.h
 * 
 * The abstract Backend object
 * 
*/


#pragma once

#include "event_listener.h"
#include "utils/expected.h"
#include "archway_types.h"
#include "yaml-cpp/yaml.h"
#include <string>
#include <drogon/HttpClient.h>


namespace archway {


  class Backend : public EventListener {

    public:

      Backend() = delete;

      Backend( 
        const std::string& in_name) : name_(in_name) {

      }

      /**
       * !! Deprecated: Use the 'Fetch' method instead
      */
      virtual std::string ActiveAccessPath() = 0;

      
      /**
       * @brief The 'FetchResponseHandler' calls this memebr function.
       *  The result of the Fetch process along with the status code
       *  will be put into the 'Message' parameter of the `callback`.
       * @param in_http_client_factory A factory function which creates a HttpClient instance
       * @param in_message Input message, which contains the original `request`
       * @param did_fetch_callback The callback gets called when the 'fetch' processes completes
       * with success or failure.
      */
      virtual Expected<void> Fetch(
        std::function<drogon::HttpClientPtr(const std::string&)> in_http_client_factory,
        std::shared_ptr<Message> & in_message,
        std::function<Expected<void>(std::shared_ptr<Message>&)> did_fetch_callback
      ) = 0;

      const std::string& Name() const {
        return name_;
      }


      virtual ~Backend() {}

    private:

      const std::string name_;

  };
}