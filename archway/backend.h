/**
 * backend.h
 * 
 * The abstract Backend object
 * 
*/


#pragma once

#include "event_listener.h"
#include "yaml-cpp/yaml.h"
#include <string>


namespace archway {


  class Backend : public EventListener {

    public:

      Backend() = delete;

      Backend( 
        const std::string& in_name) : name_(in_name) {}

      virtual std::string ActiveAccessPath() = 0;

      const std::string& Name() {
        return name_;
      }

    private:

      const std::string name_;

  };
}