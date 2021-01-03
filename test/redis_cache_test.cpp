#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <iostream>
#include <thread>
#include <drogon/drogon.h>
#include "redis_cache.h"





class RedisCacheTest : public ::testing::Test {

  protected:

    

    static void SetUpTestCase() {
      
      std::cout << "==== RedisCacheTest::SetUpTestCase\n";

      web_thread_ = std::make_shared<std::thread>( [] {

        std::cout << "Starting the web server in another thread....\n";
        drogon::app().addListener("0.0.0.0",7671);
        drogon::app().run();
        
      });
      
      // Let's have an intermission to make sure trantor event loop
      //  has been started so far.
      std::this_thread::sleep_for( std::chrono::milliseconds(10));

      redis_cache_ = std::make_shared<archway::RedisCache>();

      // Since our implementation of 'redis::connect' is async, we 
      //  should wait here until the connection get established

      std::this_thread::sleep_for( std::chrono::milliseconds(100));
    }


    static void TearDownTestCase() {

      std::cout << "==== RedisCacheTest::TearDownTestCase\n";
      
      // This sleep is mandatory for app().quit() to function correctly!
      std::this_thread::sleep_for( std::chrono::milliseconds(10));
      drogon::app().quit();
      
      web_thread_->join();
      
      std::cout << "------ AFTER completing the web thread!\n";
    }

  

    // C++17 feature: We don't need static defs anymore!
    inline static std::shared_ptr<std::thread> web_thread_;
    inline static std::shared_ptr<archway::RedisCache> redis_cache_;
    std::mutex mutex_;
    bool was_done_ = false;
    std::condition_variable condition_;
};



TEST_F(RedisCacheTest, Lookup) {

  auto the_cache_status = RedisCacheTest::redis_cache_->Status();
  EXPECT_EQ( the_cache_status, archway::CacheStatus::kReady);

  bool is_lookup_ok{false};

  if( the_cache_status == archway::CacheStatus::kReady ) {

    auto the_call_status = RedisCacheTest::redis_cache_->Lookup(
      "archway.hayoola.host/index.js",
      [this, &is_lookup_ok](
        archway::LookupStatus in_lookup_status,
        const std::string& in_value
      ) {

        std::string the_status_string{""};
        
        switch( in_lookup_status ) {

          case archway::LookupStatus::kInvalidValue:
            the_status_string = "Invalid value(!!)";
            break;

          case archway::LookupStatus::kNotFound:
            the_status_string = "Value didn't found";
            is_lookup_ok = true;
            break;

          case archway::LookupStatus::kInProgress:
            the_status_string = "Value is in-progress";
            is_lookup_ok = true;
            break;

        }

        std::cout << "**** The Lookup completed. status: " << the_status_string << std::endl;
        was_done_ = true;
        condition_.notify_one();
      }
    );

    std::unique_lock<std::mutex> the_lock(mutex_);
    condition_.wait( the_lock, [this] {
      return was_done_;
    });

    the_lock.unlock();

    EXPECT_TRUE( is_lookup_ok); 
  }
}




int main(int argc, char **argv)
{
    //testing::InitGoogleTest(&argc, argv);
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}