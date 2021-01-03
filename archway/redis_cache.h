/**
 * redis_cache.h
 * 
 * Redis cache interface
*/

#pragma once


#include <string>
#include <memory>
#include <map>
#include <cpp_redis/cpp_redis>
#include "utils/expected.h"
#include "cache.h"



namespace archway {


  class RedisCache {

    public:

      RedisCache(
        const std::string& in_host = "127.0.0.1",
        const size_t in_port = 6379
      );

      Expected<void> Lookup( 
        const std::string& in_key,
        LookupCallback in_reply_callback
      );


      Expected<void> Store(
        const std::string& in_key,
        const std::string& in_value,
        int in_ttl,
        StoreCallback in_store_callback
      );

      CacheStatus Status() {
        return redis_status_;
      }



    private:

      using RegisterScriptCallback = std::function<void(const std::string& in_script_sha1)>;

      
      /**
       * A helper class for 'loading' lua scripts into the Redis instance
      */
      class LuaScriptRegisterer {

        public:

          LuaScriptRegisterer(
            std::shared_ptr<cpp_redis::client> in_redis_client
          ) : redis_(in_redis_client) {

          }
          
          Expected<void> Register(
            const std::string& in_script,
            RegisterScriptCallback in_register_callback
          );

        private:
          std::shared_ptr<cpp_redis::client> redis_;

      };

      /*
      static LookupStatus std::map<std::string, LookupStatus> s_string_to_lookup_status_map {
        {"IV", LookupStatus::kInvalidValue}

      };
      */

      static LookupStatus string_to_lookup_status( std::string in_string ) {

        LookupStatus the_status{LookupStatus::kInvalidValue};

        const static std::map<std::string, LookupStatus> s_string_to_lookup_status_map {
          {"IR", LookupStatus::kInvalidValue},
          {"NF", LookupStatus::kNotFound},
          {"IP", LookupStatus::kInProgress},
          {"FV", LookupStatus::kFound}
        };

        auto the_iter = s_string_to_lookup_status_map.find(in_string);
        if( the_iter != s_string_to_lookup_status_map.end() ) {
          the_status = the_iter->second;
        }

        return the_status;
      }

      std::shared_ptr<cpp_redis::client> redis_;
      std::shared_ptr<LuaScriptRegisterer> script_registerer_;
      CacheStatus redis_status_;
      std::string lookup_script_sha1_;
      const static std::string lookup_script_source_;
  };

}