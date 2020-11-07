/**
 * archway.h
 * 
 * The main orchestractor and vital data storage of the project.
 * 
**/


#pragma once


#include <string>
#include <drogon/HttpClient.h>
#include "backend.h"
#include "host_program.h"
#include "router.h"



namespace archway {

  class Archway : public std::enable_shared_from_this<Archway> {

    public:

      Archway( 
        std::function<drogon::HttpClientPtr(const std::string&)> in_http_client_factory 
      );

      
      std::shared_ptr<Archway> shared_ptr() {
        return shared_from_this();
      }
      
      void AddBackend(const std::shared_ptr<Backend> in_backend) {
        
        router_->AddBackend(in_backend);
      }


      int GetBackendIndex(const std::string& in_name) {

        return router_->GetBackendIndex(in_name);
      }

      
      
      std::shared_ptr<Backend> GetBackend( int in_index) {

        return router_->GetBackend(in_index);
      }

      
      
      void AddHostProgram(
        const std::vector<std::string>& in_host_names,
        const std::shared_ptr<HostProgram> in_host_program
      ) {
        router_->AddHostProgram(in_host_names, in_host_program);
      }


      std::shared_ptr<HostProgram> FindHostProgram(
        const std::string& in_host_name
      ) {
        return router_->FindHostProgram(in_host_name);
      }

      

    private:

      std::shared_ptr<Router> router_;

  };
}