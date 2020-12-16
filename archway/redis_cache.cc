/**
 * redis_cache.cc
*/


#include "redis_cache.h"
#include "utils/redis_trantor_client.h"
#include <trantor/utils/Logger.h>


using namespace archway;



RedisCache::RedisCache(
  const std::string& in_host,
  const size_t in_port
) : 
  redis_status_(CacheStatus::kStarting) {

  auto the_redis_tcp_adapter = std::make_shared<archway::RedisTrantorClient>();
  redis_status_ = CacheStatus::kConnecting;
  LOG_DEBUG << "Starting Redis connection...";

  the_redis_tcp_adapter->set_on_connected_handler( [this](bool in_was_successful ) {

    if( in_was_successful ) {
      this->redis_status_ = CacheStatus::kReady;
      LOG_DEBUG << "Redis connection established!";
    
    } else {
      this->redis_status_ = CacheStatus::kFailed;
    }
    
  });

  redis_ = std::make_shared<cpp_redis::client>(the_redis_tcp_adapter);
  redis_->connect( in_host, in_port); // Our implementation is non-blocking, so we exit here
}