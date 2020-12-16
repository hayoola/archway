/**
 * cache.cc
 * 
*/

#include "cache.h"
#include "redis_cache.h"



using namespace archway;



Cache::Cache(
  const std::string& in_redis_host,
  const size_t in_redis_port
) {

  redis_cache_ = std::make_shared<RedisCache>(in_redis_host, in_redis_port);

}





Expected<void> Cache::Lookup( 
  const std::string& in_key,
  LookupCallback in_reply_callback
) {

  return {};

}





Expected<void> Cache::Store(
  const std::string& in_key,
  const std::string& in_value,
  StoreCallback in_store_callback
) {

  return {};
  
}


CacheStatus Cache::Status() {
  return redis_cache_->Status();
}