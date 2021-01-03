/**
 * cache.h
 * 
 * Archway caching orchestrator
 * 
*/


#pragma once


#include <string>
#include <functional>
#include <memory>
#include "utils/expected.h"


namespace archway {

  enum class CacheStatus {
    kStarting,
    kConnecting,
    kReady,
    kFailed
  };

  enum class LookupStatus {
    kInvalidValue = 0,
    kNotFound,
    kInProgress,
    kFound
  };
  
  using LookupCallback = 
    std::function<void(
      LookupStatus in_lookup_status,
      const std::string& in_value
    )>
  ;

  using StoreCallback = 
    std::function<Expected<void>( bool in_was_successful)>
  ;

  


  class MemoryCache;
  class RedisCache;

  class Cache {

    public:

      Cache(
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
        StoreCallback in_store_callback = nullptr
      );

      CacheStatus Status();

    private:

      std::shared_ptr<MemoryCache> memory_cache_;
      std::shared_ptr<RedisCache> redis_cache_;

  };

}