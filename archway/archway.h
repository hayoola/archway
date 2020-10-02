/**
 * archway.h
 * 
 * The main orchestractor and vital data storage of the project.
 * 
**/


#pragma once


#include <unordered_map>
#include <string>
#include "backend.h"



namespace archway {

  class Archway {

    public:

      Archway();

      void AddBackend(const std::shared_ptr<Backend> in_backend) {

        backends_.emplace(
          in_backend->Name(),
          in_backend
        );
      }


      std::shared_ptr<Backend> GetBackend(const std::string& in_name) {

        std::shared_ptr<Backend> the_backend_ptr{nullptr};

        auto the_iter = backends_.find(in_name);
        if( the_iter != backends_.end() ) {
          the_backend_ptr = the_iter->second;
        }

        return the_backend_ptr;
      }


    private:
      std::unordered_map<std::string, std::shared_ptr<Backend>> backends_;
      //std::vector<HostProgram> host_programs_;

  };
}