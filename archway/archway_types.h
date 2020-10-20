/**
 *
 *  archway_types.h
 * 
 *  Archway general types
 *
*/

#pragma once

#include <variant>
#include <string>
#include <unordered_map>
#include <drogon/HttpRequest.h>
#include <drogon/HttpResponse.h>

namespace archway {

  class HostProgram;

  /**
   * The code, which returns from archway::DynamicFunctions
  */
  /*
  enum class Action {
    kFail,    // Fatal Internal (archway) error: deliver a short error response
    kError,   // ==> Redundant: 
    kPass,
    kPipe,
    kHash,
    kPurge,
    kRestart,
    kFetch,
    kDeliver,
    _kLast    //dummy item to get the number of items
    // !! Don't insert any item after _kLast
  };
  */



  /**
   * The definition of different stages during the request's journey from admission 
   * into the proxy to departure from it and head to the client.
   * Corresponding to each stage, an `Archway::DynamicFunction` can be defined to
   * catch the request or response and processes it.
  */
  enum class Stage {
    kStart,
    kReceive,
    kPipe,
    kPass,
    kHash,
    kPurge,
    kMiss,
    kHit,
    kBackendFetch,
    kBackendResponse,
    kBackendError,
    kDeliver,
    kEnd,
    _kLast    //dummy item to get the number of items
    // !! Don't insert any item after _kLast
  };

  enum class ParamID {
    kStage,   // The next stage which is proposed by staged functions
    kBackendIndex
  };

  using RequestResponsePtr = std::variant<
    drogon::HttpRequestPtr,
    drogon::HttpResponsePtr
  >;

  using IntString = std::variant<
    int,
    std::string
  >;

  
  /**
   * The archway umbrella data structure which holds request or response,
   *  along with a map containing archway-specific parameters.
   * The parameters can be numeric or string, thanks to std::variant.
   * This structure will pass to all DynamicFunction objects.
  */ 
  class Message {

    public:
      
      Message() = delete;

      
      /**
       * The initial constructor.
       * In the initial stages of the routing process we deal with
       *  request, so firstly we create a Message bearing the Request.
      */
      Message( const drogon::HttpRequestPtr& in_request) :
        req_resp_(in_request) {
        
        params_.emplace(ParamID::kStage, static_cast<int>(Stage::kStart));
      }

      
      /**
       * Converting constructor.
       * After kBackendFetch stage, we need to put the fetched response
       *  from backend into the message, so here we construct a copy of
       *  input Message, which bears a Response object from now on.
      */
      Message( 
        const Message& in_message,
        const drogon::HttpResponsePtr& in_response
      ) :
        req_resp_(in_response) {
        
        params_ = in_message.params_;
      }

      
      
      drogon::HttpRequestPtr Request() {

        auto the_ptr = std::get_if<drogon::HttpRequestPtr>(&req_resp_);
        if( the_ptr ) {
          return *the_ptr;
        
        } else {
          return nullptr;

        }
      }

      drogon::HttpResponsePtr Response() {

        auto the_ptr = std::get_if<drogon::HttpResponsePtr>(&req_resp_);
        if( the_ptr ) {
          return *the_ptr;

        } else {
          return nullptr;

        }
      }


      std::shared_ptr<HostProgram> &host_program() {
        return host_program_;
      }


      /**
       * The string version. 
       * Add or update the param designated by the input id
       * @param in_id The numeric id of the param
       * @param in_data The textual value of the param
      */
      void SetParam(
        ParamID in_id,
        const std::string& in_data
      ) {

        auto the_iter = params_.find(in_id);
        if( the_iter != params_.end()) {
          the_iter->second = in_data;

        } else {
          params_.emplace(in_id, in_data);

        }
      }


      /**
       * The numeric version. 
       * 
       * Add or update the param designated by the input id
       * @param in_id The numeric id of the param
       * @param in_data The numeric value of the param
      */
      void SetParam(
        ParamID in_id,
        const int in_data
      ) {

        auto the_iter = params_.find(in_id);
        if( the_iter != params_.end()) {
          the_iter->second = in_data;

        } else {
          params_.emplace(in_id, in_data);

        }
      }

      
      std::string StringParam( ParamID in_id ) {

        std::string the_param{""};

        auto the_iter = params_.find(in_id);
        if( the_iter != params_.end() ) {
          
          auto the_ptr = std::get_if<std::string>(&(the_iter->second));
          if( the_ptr ) {
            the_param = *the_ptr;
          }
        }

        return the_param;
      }


      
      int IntParam( ParamID in_id ) {

        int the_param{-1};

        auto the_iter = params_.find(in_id);
        if( the_iter != params_.end() ) {

          auto the_ptr = std::get_if<int>(&(the_iter->second));
          if( the_ptr ) {
            the_param = *the_ptr;
          }
        }

        return the_param;
      }

    private:

      RequestResponsePtr req_resp_;
      std::shared_ptr<HostProgram> host_program_;
      std::unordered_map<ParamID, IntString> params_;

  };

}