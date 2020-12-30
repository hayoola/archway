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

  using LookupCallback = 
    std::function<Expected<void>(
      // A prior request has been made for this key, but the value is not ready, yet
      bool is_in_progress,
      const std::string& in_reply
    )>
  ;

  using StoreCallback = 
    std::function<Expected<void>( bool in_was_successful)>
  ;

  enum class CacheStatus {
    kStarting,
    kConnecting,
    kReady,
    kFailed
  };


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