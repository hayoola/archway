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


namespace archway {


  class Backend : public EventListener {

    public:

      Backend() = delete;

      Backend( 
        const std::string& in_name) : name_(in_name) {}

      virtual std::string ActiveAccessPath() = 0;

      
      /**
       * @brief The 'FetchResponseHandler' calls this memebr function.
       * The callback gets called when the 'fetch' processes completes
       *  with success or failure. The status along with the status code
       *  will be put into the 'Message' parameter of the callback.
      */
      virtual Expected<void> Fetch(
        std::shared_ptr<Message> & in_message,
        std::function<Expected<void>(std::shared_ptr<Message>&)> did_fetch_callback
      ) = 0;

      const std::string& Name() const {
        return name_;
      }

    private:

      const std::string name_;

  };
}