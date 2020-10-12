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
#include "host_program.h"
#include "router.h"



namespace archway {

  class Archway {

    public:

      Archway();

      void AddBackend(const std::shared_ptr<Backend> in_backend) {
        
        // In order to make the life for `Compiler` and ourselves easier,
        //  we put the Backend into a vector and then insert the
        //  index to that vector's entry into a map.
        // In this way, the Compiler can lookup the index by name and 
        //  then use that index to construct a `SetBackendInstruction`
        //  object properly.
        
        backends_vector.emplace_back(in_backend);
        backends_map_.emplace(in_backend->Name(), backends_vector.size() - 1);
      }


      int GetBackendIndex(const std::string& in_name) {

        int the_index{-1};

        auto the_iter = backends_map_.find(in_name);
        if( the_iter != backends_map_.end() ) {
          the_index = the_iter->second;
        }

        return the_index;
      }

      
      
      std::shared_ptr<Backend> GetBackend( int in_index) {

        std::shared_ptr<Backend> the_backend{nullptr};

        if( in_index >= 0 && in_index < backends_vector.size()) {

          the_backend = backends_vector[in_index];
        }

        return the_backend;
      }

      
      
      void AddHostProgram(
        const std::vector<std::string>& in_host_names,
        const std::shared_ptr<HostProgram> in_host_program
      ) {
        router_.AddHostProgram(in_host_names, in_host_program);
      }

      

    private:

      std::unordered_map<std::string, int> backends_map_;
      std::vector<std::shared_ptr<Backend>> backends_vector;
      Router router_;

  };
}