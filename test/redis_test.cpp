#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>
#include <exception>
#include <cpp_redis/cpp_redis>
#include <drogon/drogon.h>
#include <thread>
#include "utils/redis_trantor_client.h"








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
      client_ = std::make_shared<cpp_redis::client>(the_redis_tcp_adapter);
      
      client_->connect("127.0.0.1", 6379, [](
        const std::string& in_host, 
        std::size_t in_port, 
        cpp_redis::connect_state in_status) {
        
        if (in_status == cpp_redis::connect_state::dropped) {
          
          std::cout << "Client dropped from " << in_host << ":" << in_port << std::endl;
        
        } else if( in_status == cpp_redis::connect_state::start ) {

          std::cout << "Starting to connect to Redis ..." << std::endl;
        
        } else if( in_status == cpp_redis::connect_state::ok ) {

          std::cout << "Redis Connection established!" << std::endl;
          
        }
      
      });

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