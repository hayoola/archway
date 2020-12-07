/**
 * main.cc
 * 
 * Archway main source file
 * 
 */


#include <drogon/drogon.h>
#include <cpp_redis/cpp_redis>
#include <iostream>
#include "archway.h"
#include "utils/redis_trantor_client.h"



int main() {
    
    auto the_archway = archway::Archway::Create(
      
      [](const std::string& in_host_address) { 
        
        return drogon::HttpClient::newHttpClient(
          in_host_address,
          trantor::EventLoop::getEventLoopOfCurrentThread()
        );
      }
    );

    LOG_DEBUG << "The Archway object got constructed.";


    auto the_compile_result = the_archway->Compile("config.yaml");
    if( the_compile_result.wasSuccessful()) {
      
      LOG_DEBUG << "The Archway config file was compiled successfully!";
    } else {

      LOG_DEBUG << "Error: " << the_compile_result.error();
      abort();
    }

    drogon::app().registerPreRoutingAdvice(
      [the_archway](
        const drogon::HttpRequestPtr &req,
        drogon::AdviceCallback &&callback,
        drogon::AdviceChainCallback &&pass
      ) {
      
        LOG_DEBUG << "====================\n" << "New request... Path: " << req->getPath();

        auto the_route_result = the_archway->Route(
          req,
          std::move(callback)
        );
      }

    );

    auto the_redis_tcp_adapter = std::make_shared<archway::RedisTrantorClient>();
    the_redis_tcp_adapter->set_on_connected_handler( [](bool in_was_successful ) {

      if( in_was_successful ) {
        LOG_DEBUG << "Redis connection was established!";
      
      } else {
        LOG_DEBUG << "Can't connect to Redis!";
      }
      
    });

    auto the_client = std::make_shared<cpp_redis::client>(the_redis_tcp_adapter);

    the_client->connect("127.0.0.1", 6379);


    drogon::app().registerBeginningAdvice(
      [the_client]() { 
        
        LOG_DEBUG << "Event loop was started";

        if( the_client->is_connected() ) {
          LOG_DEBUG << "Redis is ready.";
        
        } else {
          LOG_DEBUG << "Redis has not ready yes!";
          // TODO: Devise a timer-like mechanism to check for connection after a timeout
        }

      }
    );
    
    
    // Set HTTP listener address and port
    // TODO: Use our own general config file
    drogon::app().addListener("0.0.0.0",7671);
    
    //Load drogon config file
    //drogon::app().loadConfigFile("./config.json");
    //std::cout << "The drogon config file was parsed.\n";
    
    
    // Run HTTP framework,the method will block in the internal event loop
    LOG_DEBUG << "Starting Archway ....";

    drogon::app().run();

    return 0;
}
