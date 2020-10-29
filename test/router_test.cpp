#include <gtest/gtest.h>
#include <drogon/HttpRequest.h>
#include <iostream>
#include "router.h"
#include "archway.h"
#include "single_host_backend.h"



using namespace archway;




TEST(Archway_Router_Test, HostProgram_Lookup) {

  Router the_router;
  auto  the_host_program = std::make_shared<HostProgram>();
  std::vector<std::string> the_host_names{"*.example.com", "*.think.com"};
  the_router.AddHostProgram(the_host_names, the_host_program);

  auto the_found_program = the_router.FindHostProgram("www.example.com");
  EXPECT_NE( the_found_program, nullptr);
  EXPECT_EQ( the_found_program, the_host_program);
}



TEST(Archway_Router_Test, Backend_Lookup) {

  Router the_router;
  auto the_name{"base"};
  auto the_backend = std::make_shared<SingleHostBackend>(the_name, "http://127.0.0.1:8088");
  the_router.AddBackend(the_backend);

  int the_index = the_router.GetBackendIndex(the_name);
  EXPECT_EQ(the_index, 0);

  auto the_found_backend = the_router.GetBackend(the_index);
  EXPECT_EQ(the_found_backend->Name(), the_name);
}




TEST(Archway_Router_Test, Basic_Routing) {

  try {

    // We can't construct a stack-based object here,
    //  because we want to use std::make_shared_from_this()
    auto the_router = std::make_shared<Router>();
  
    auto  the_host_program = std::make_shared<HostProgram>();
    std::vector<std::string> the_host_names{"*.example.com", "*.think.com"};
    the_router->AddHostProgram(the_host_names, the_host_program);
    
    auto the_name{"base"};
    auto the_backend = std::make_shared<SingleHostBackend>(the_name, "http://127.0.0.1:8088");
    the_router->AddBackend(the_backend);

    drogon::HttpRequestPtr  the_request = drogon::HttpRequest::newHttpRequest();
    the_request->addHeader("host", "www.example.com");
    
    auto the_result = the_router->Route(the_request);
    EXPECT_TRUE(the_result.wasSuccessful());


  } catch( const std::exception& in_error ) {

    std::cout << "Router execution failed!" << in_error.what();
    FAIL();
  }
  
  
}






int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}