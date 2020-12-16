/**
 * redis_cache.h
 * 
 * Redis cache interface
*/

#pragma once


#include <string>
#include <memory>
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
        StoreCallback in_store_callback
      );

      CacheStatus Status() {
        return redis_status_;
      }



    private:

      std::shared_ptr<cpp_redis::client> redis_;
      CacheStatus redis_status_;
  };

}