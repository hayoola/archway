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
#include "utils/expected.h"



namespace archway {

  class Compiler;

  class Archway : public std::enable_shared_from_this<Archway> {

    public:

      /**
       * The Archway factory method
      */
      static std::shared_ptr<Archway> Create(
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


      /**
       * Facet memebr function for the Compiler object
      */
      Expected<void> Compile(
        const std::string& in_file_name
      );


      /**
       * Facet member function for the Router object
      */
      Expected<void> Route(
        const drogon::HttpRequestPtr& in_request,
        drogon::AdviceCallback&& in_drogon_advice_callback
      );

    protected:

      /**
       * Protected constructor to prevent direct instantiation
       * Use the static `Create` factory function, instead.
      */
      Archway() :
      router_(nullptr), compiler_(nullptr) {

      }

    private:

      std::shared_ptr<Router> router_;
      std::shared_ptr<Compiler> compiler_;
  };
}