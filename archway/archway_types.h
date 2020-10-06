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

  /**
   * The code, which returns from archway::Functions
  */
  enum class Action {
    kInvalid,
    kContinue,
    kFail,    // Fatal Internal (archway) error: deliver a short error response
    kError,   // ==> Redundant: 
    kPass,
    kPipe,
    kHash,
    kPurge,
    kRestart,
    kFetch,
    kDeliver
  };



  /**
   * The definition of different stages during the request's journey from admission 
   * into the proxy to departure from it and head to the client.
   * Corresponding to each stage, an `Archway::Function` can be defined to
   * catch the request or response and processes it.
  */
  enum class Stage {
    kInvalid,
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
    _kLast    //dummy item to get the number of items
    // !! Don't insert any item after _kLast
  };

  enum class ParamID {
    kAction,
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
   * This structure will pass to all Function objects.
  */ 
  class Message {

    public:
      
      Message() = delete;

      Message( const drogon::HttpRequestPtr& in_request) :
        req_resp_(in_request) {

      }

      Message( const drogon::HttpResponsePtr& in_response) :
        req_resp_(in_response) {

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
      std::unordered_map<ParamID, IntString> params_;

  };

}