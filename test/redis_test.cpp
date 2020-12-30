#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>
#include <exception>
#include <cpp_redis/cpp_redis>
#include <drogon/drogon.h>
#include <thread>
#include "utils/redis_trantor_client.h"
#include "sha1/sha1.hpp"







class RedisTest : public ::testing::Test {

  protected:

    

    static void SetUpTestCase() {
      
      std::cout << "==== RedisTest::SetUpTestCase\n";

      web_thread_ = std::make_shared<std::thread>( [] {

        std::cout << "Starting the web server in another thread....\n";
        drogon::app().addListener("0.0.0.0",7671);
        drogon::app().run();
        
      });
      
      // Let's have an intermission to make sure trantor event loop
      //  has been started so far.
      std::this_thread::sleep_for( std::chrono::milliseconds(10));

      auto the_redis_tcp_adapter = std::make_shared<archway::RedisTrantorClient>();
      the_redis_tcp_adapter->set_on_connected_handler( [](bool in_was_successful ) {

        if( in_was_successful ) {
          
          std::cout << "Redis connection established!\n";
        
        } else {
          
          std::cout << "Can't connect to Redis!\n";
        }
      
      });
      
      client_ = std::make_shared<cpp_redis::client>(the_redis_tcp_adapter);
      
      client_->connect("127.0.0.1", 6379);

      // Since our implementation of 'connect' is async, we 
      //  should wait here until the connection get established

      std::this_thread::sleep_for( std::chrono::milliseconds(100));

    }


    static void TearDownTestCase() {

      std::cout << "==== RedisTest::TearDownTestCase\n";
      
      // This sleep is mandatory for app().quit() to function correctly!
      std::this_thread::sleep_for( std::chrono::milliseconds(10));
      drogon::app().quit();
      
      web_thread_->join();
      
      std::cout << "------ AFTER completing the web thread!\n";
      client_->disconnect();
    }

  

    void SetUp() override {
      
      //std::cout << "RedisTest::SetUp begins ...\n";
    }

    
    
    void TearDown() override {

      //std::cout << "RedisTest::TearDown ...\n";
    }

    
    // C++17 feature: We don't need static defs anymore!
    inline static std::shared_ptr<cpp_redis::client> client_; 
    inline static std::shared_ptr<std::thread> web_thread_;
    std::mutex mutex_;
    bool was_done_ = false;
    std::condition_variable condition_;

};



TEST_F(RedisTest, SetGet) {

  
  SUCCEED();

  
  try {

    
    was_done_ = false;

    RedisTest::client_->set("archway:test", "767", [](cpp_redis::reply& in_reply) {

      std::cout << "The SET reply from Redis: " << in_reply << std::endl;

    });

    client_->sync_commit();

    std::string the_script_sha1{""};
    const std::string the_script{ 
      R"(
        local t = { KEYS[1], ARGV[1] };
        return t;
      )"
    };

    SHA1 the_checksum;
    the_checksum.update(the_script);
    const std::string the_calculated_sha1 = the_checksum.final();

    RedisTest::client_->script_exists(
      {
        the_calculated_sha1
      },
      [](cpp_redis::reply& in_reply) {

        std::cout << "The SCRIPT EXISTS reply from Redis: " << in_reply << std::endl;
        if( in_reply.is_array() ) {

          auto the_value = in_reply.as_array()[0].as_integer();
          std::cout << "-------> EXISTS value: " << the_value << std::endl;
        }
      }
    );
    
    RedisTest::client_->script_load(
      the_script,
      [&the_script_sha1](cpp_redis::reply& in_reply) {

        std::cout << "The SCRIPT LOAD reply from Redis: " << in_reply << std::endl;

        if( in_reply.is_string() ) {

          the_script_sha1 = in_reply.as_string();
        }

      }
    );

    client_->sync_commit();

    std::cout << "The script SHA1: " << the_script_sha1 << std::endl;

    if( the_script_sha1 != "" ) {

      RedisTest::client_->evalsha(
        the_script_sha1,
        {
          "archway.hayoola.host/index.js"
        },
        {
          "1000"
        },
        [](cpp_redis::reply& in_reply) {

          std::cout << "\nThe ECALSHA reply from Redis: " << in_reply << std::endl;
        }
      );

    }

    

    RedisTest::client_->eval(
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
      )",
      {
        "archway.hayoola.host/index.js"
      },
      {
        "1000",
      },
      [](cpp_redis::reply& in_reply) {

        std::cout << "The EVAL reply from Redis: " << std::endl;

        std::cout << "-------------\n";

        if( in_reply.is_array() ) {

          auto the_reply = in_reply.as_array();
          for( auto& the_item : the_reply) {

            int dummy = -65;
            std::cout << the_item.as_string() << " - ";
            
          }
        }
        
        std::cout << "\n-------------\n";
      }
    );

    client_->sync_commit();
    
    
    client_->get("archway:test", [this](cpp_redis::reply& in_reply) {

      std::cout << "The GET reply from Redis: " << in_reply << std::endl;

      EXPECT_TRUE(in_reply.is_string());
      EXPECT_EQ(in_reply.as_string(), "767");
      
      {
        //std::lock_guard<std::mutex> the_lock(g_mutex);
        was_done_ = true;
      }

      condition_.notify_one();

    });

    // std::cout << "About to call `sync_commit()`" << std::endl;
    // the_redis_client.sync_commit();
    // std::cout << "AFTER the call of `sync_commit()`" << std::endl;
    
    std::cout << "BEFORE commit()" << std::endl;
    client_->commit();

    std::cout << "BEFORE waiting for done..." << std::endl;
    
    std::unique_lock<std::mutex> the_lock(mutex_);
    condition_.wait( the_lock, [this] {
      return was_done_;
    });

    std::cout << "AFTER wait!" << std::endl;
    the_lock.unlock();

  } catch( std::exception& in_error ) {

    std::cout << "Redis connection failed!" << in_error.what();
    FAIL();
  }

  

}



TEST_F(RedisTest, Test2) {

  int dummy = 767;

  EXPECT_EQ(dummy, 767);
}




int main(int argc, char **argv)
{
    //testing::InitGoogleTest(&argc, argv);
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}