/**
 * redis_cache.cc
*/


#include "redis_cache.h"
#include "utils/redis_trantor_client.h"
#include <trantor/utils/Logger.h>
#include "sha1/sha1.hpp"


using namespace archway;



const std::string RedisCache::lookup_script_source_{
  R"(
    -- local t = { KEYS[1], ARGV[1] };
    local t = {};
    local result = -1;
    local value = "";

    local key = KEYS[1];
    local in_progress_ttl = ARGV[1];

    -- t[1] = key;
    -- t[2] = in_progress_ttl;
    -- t = {key, in_progress_ttl};

    local status = redis.call('hget', key, "s");
    if status == false then -- redis.call() NEVER returns 'nil'!
      -- create an 'in-progress' record
      local redis_result = redis.call('hset', key, "s", "i");
      if redis_result == 1 then
        result = "NF";  -- Not Found

        -- make the record transient
        redis_result = redis.call('expire', key, in_progress_ttl);
        if redis_result ~= 1 then
          result = -2;
        end
      
      end
    
    elseif status == "i" then
      result = "IP"; -- In Progress
    
    elseif status == "c" then
      -- A valid record found, so get the value
      value = redis.call('hget', key, "v");
      if value == nil then
        result = -3;
      
      else
        result = "FV"; -- Found Value
      
      end

    else
      result = "IR"; -- Invalid Record
      -- Remove the invalid record
      -- TODO
    end

    t[1] = result;
    t[2] = value;
    t[3] = key;
    t[4] = in_progress_ttl;
    --t[5] = status;

    return t;
  )"
};





RedisCache::RedisCache(
  const std::string& in_host,
  const size_t in_port
) : 
  redis_status_(CacheStatus::kStarting),
  lookup_script_sha1_("") {

  
  auto the_redis_tcp_adapter = std::make_shared<archway::RedisTrantorClient>();
  redis_status_ = CacheStatus::kConnecting;
  LOG_DEBUG << "Starting Redis connection...";

  the_redis_tcp_adapter->set_on_connected_handler( [this](bool in_was_successful ) {

    if( in_was_successful ) {
      // this->redis_status_ = CacheStatus::kReady;
      LOG_DEBUG << "Redis connection established!";

      script_registerer_ = std::make_shared<LuaScriptRegisterer>(redis_);
      
      script_registerer_->Register(
        lookup_script_source_,
        [this](const std::string& in_script_sha1) {
          
          this->lookup_script_sha1_ = in_script_sha1;
          this->redis_status_ = CacheStatus::kReady;
          LOG_DEBUG << "Redis Lookup script got registered!";
        }
      );
    
    } else {
      this->redis_status_ = CacheStatus::kFailed;
    }
    
  });

  redis_ = std::make_shared<cpp_redis::client>(the_redis_tcp_adapter);
  redis_->connect( in_host, in_port); // Our implementation is non-blocking, so we exit here
}





Expected<void> RedisCache::Lookup( 
  const std::string& in_key,
  LookupCallback in_reply_callback
) {

  Expected<void> the_result{};

  if( lookup_script_sha1_ != "" ) {

    redis_->evalsha(
      lookup_script_sha1_,
      {
        in_key
      },
      {
        "1000"  // TODO: Use a configurable value for 'in-progress TTL
      },
      [in_reply_callback](cpp_redis::reply& in_reply) {

        LookupStatus the_status{LookupStatus::kInvalidValue};
        std::string the_value{""};
        
        if( in_reply.is_array() ) {

          auto the_reply = in_reply.as_array();

          auto the_result = the_reply[0].as_string();
          the_status = RedisCache::string_to_lookup_status( the_result);
          
          if( the_status == LookupStatus::kFound ) {
            the_value = the_reply[1].as_string();
          }
        }

        in_reply_callback( the_status, the_value);

      }
    );

    redis_->commit();

  } else {

    the_result = CacheError("RedisCache lookup script hasn't been registered yet!");
  }

  return the_result;
}





/**
 * Asynchronously store 'in_value' (likely to be an HTTP response), which
 *  is retrievable by 'in_key' (likely to be 'host_address' + 'request_path')
*/
Expected<void> RedisCache::Store(
  const std::string& in_key,
  const std::string& in_value,
  int in_ttl,
  StoreCallback in_store_callback
) {

  Expected<void> the_result{};

  redis_->hmset(
    in_key,
    {
      std::make_pair("s", "c"),       // s: status i = 'in progress', c = 'complete'
      std::make_pair("v", in_value)
    },
    [](cpp_redis::reply& in_reply) {
        
     
    }
  );

  redis_->expire(in_key, in_ttl, [in_store_callback](cpp_redis::reply& in_reply) {

    bool the_store_result{false};

    if( in_reply.as_string() == "OK" ) {

      the_store_result = true;
    }

    in_store_callback( the_store_result);

  });

  redis_->commit();


  return the_result;
}


/**
 * Register the provided script with the Redis instance, 
 *  if it hasn't been registered already. 
 * Calls 'in_register_callback' when it finishes and passes
 *  the 'hash' value of the script to the callback. This 
 *  'hash' can be used to 'Run' the script inside Redis
 *  via the 'evalsha' call
 * 
 * @param in_script The Lua script source
 * @param in_register_callback On completion callback
*/
Expected<void> RedisCache::LuaScriptRegisterer::Register(
  const std::string& in_script,
  RegisterScriptCallback in_register_callback
) {

  Expected<void> the_result{};

  SHA1 the_checksum;
  the_checksum.update(in_script);
  const std::string the_calculated_sha1 = the_checksum.final();

  redis_->script_exists(
    {
      the_calculated_sha1
    },
    [
      this,
      in_script,
      the_calculated_sha1,
      in_register_callback
    ](
      cpp_redis::reply& in_reply
    ) {

      bool was_existed = false;

      if( in_reply.is_array() ) {

        auto the_value = in_reply.as_array()[0].as_integer();
        if( the_value == 1 ) {
          was_existed = true;
        }
      }

      if( was_existed ) {

        in_register_callback( the_calculated_sha1);

      } else {

        redis_->script_load(
          in_script,
          [in_register_callback](cpp_redis::reply& in_reply) {

            if( in_reply.is_string() ) {

              auto the_script_sha1 = in_reply.as_string();
              in_register_callback( the_script_sha1);

            }

          }
        );

        redis_->commit();

      }

    }
  );

  redis_->commit();

  return the_result;
}


