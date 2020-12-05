#include <drogon/drogon.h>
#include <iostream>
#include "archway.h"



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
