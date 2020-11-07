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
    kRouterPtr,
    kBackendIndex,
    kHostProgram,
    kDrogonAdviseCallback,
    kFetchStatus,
    _kLast
    // !! Don't insert any item after _kLast
  };

  using RequestResponsePtr = std::variant<
    drogon::HttpRequestPtr,
    drogon::HttpResponsePtr
  >;


  
  /**
   * The archway umbrella data structure which holds request or response,
   *  along with a vector containing archway-specific parameters.
   * The parameters can be anything, thanks to std::any.
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
        
        params_.resize(static_cast<size_t>(ParamID::_kLast));
        
      }

      
      /**
       * After kBackendFetch stage, we need to put the fetched response
       *  from backend into the message.
       * Note the response will take the place of the request, so
       *  after the call to this function, the response object 
       *  will be lost (deconstructed soon by shared_ptr)
      */
      void SetResponse( const drogon::HttpResponsePtr& in_response ) {
        req_resp_ = in_response;
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


      
      std::any& parameter( ParamID in_id) {
        return params_.at(static_cast<size_t>(in_id));
      }


    private:

      RequestResponsePtr req_resp_;
      std::vector<std::any> params_;
  };

}