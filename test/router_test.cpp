#include <gtest/gtest.h>
#include <gmock/gmock.h>
#include <drogon/HttpRequest.h>
#include <drogon/HttpClient.h>
#include <iostream>
#include "router.h"
#include "archway.h"
#include "single_host_backend.h"



using namespace archway;
using ::testing::_;
using ::testing::AnyNumber;
using ::testing::An;


class MockHttpClient : public drogon::HttpClient {

  public:

    MOCK_METHOD3(sendRequest, void(
      const drogon::HttpRequestPtr &req,
      const drogon::HttpReqCallback &callback,
      double timeout
    ));

    MOCK_METHOD3(sendRequest, void(
      const drogon::HttpRequestPtr &req,
      drogon::HttpReqCallback &&callback,
      double timeout
    ));

    MOCK_METHOD1(setPipeliningDepth, void(size_t depth));
    MOCK_METHOD1(enableCookies, void(bool flag));
    MOCK_METHOD2(addCookie, void(const std::string &, const std::string &));
    MOCK_METHOD1(addCookie, void(const drogon::Cookie&));
    MOCK_METHOD0(getLoop, trantor::EventLoop*());
    MOCK_CONST_METHOD0(bytesSent, size_t());
    MOCK_CONST_METHOD0(bytesReceived,size_t());
};



class MockSingleHostBackend : public SingleHostBackend {

  public:

    MockSingleHostBackend(
      const std::string& in_name,
      const std::string& in_host_address
    ) : SingleHostBackend(in_name, in_host_address) {

    }
    
    MOCK_METHOD3(Fetch, Expected<void>(
      std::function<drogon::HttpClientPtr(const std::string&)>,
      std::shared_ptr<Message> &,
      std::function<Expected<void>(std::shared_ptr<Message>&)>
    ));


};




TEST(Archway_Router_Test, HostProgram_Lookup) {

  auto the_router = std::make_shared<Router>( 
    [](const std::string& in_address) { 
      
      //return drogon::HttpClient::newHttpClient(in_address);
      return nullptr;
    } 
  );
  auto  the_host_program = std::make_shared<HostProgram>();
  std::vector<std::string> the_host_names{"*.example.com", "*.think.com"};
  the_router->AddHostProgram(the_host_names, the_host_program);

  auto the_found_program = the_router->FindHostProgram("www.example.com");
  EXPECT_NE( the_found_program, nullptr);
  EXPECT_EQ( the_found_program, the_host_program);
}



TEST(Archway_Router_Test, Backend_Lookup) {

  auto the_router = std::make_shared<Router>( 
    [](const std::string& in_address) { 
      
      //return drogon::HttpClient::newHttpClient(in_address);
      return nullptr;
    } 
  );
  auto the_name{"base"};
  auto the_backend = std::make_shared<SingleHostBackend>(the_name, "http://127.0.0.1:8088");
  the_router->AddBackend(the_backend);

  int the_index = the_router->GetBackendIndex(the_name);
  EXPECT_EQ(the_index, 0);

  auto the_found_backend = the_router->GetBackend(the_index);
  EXPECT_EQ(the_found_backend->Name(), the_name);
}




TEST(Archway_Router_Test, Basic_Routing) {

  try {

    auto the_host_string{"http://127.0.0.1:8088"};
    auto the_mock_client = std::make_shared<MockHttpClient>();
    
    // We can't construct a stack-based object here,
    //  because we want to use std::make_shared_from_this()
    auto the_router = std::make_shared<Router>( 
      [the_mock_client](const std::string& in_address) { 
      
        return the_mock_client;
      } 
    );
  
    auto  the_host_program = std::make_shared<HostProgram>();
    std::vector<std::string> the_host_names{"*.example.com", "*.think.com"};
    the_router->AddHostProgram(the_host_names, the_host_program);
    
    auto the_name{"base"};
    auto the_backend = std::make_shared<MockSingleHostBackend>(the_name, the_host_string);
    the_router->AddBackend(the_backend);

    drogon::HttpRequestPtr  the_request = drogon::HttpRequest::newHttpRequest();
    the_request->addHeader("host", "www.example.com");
    
    EXPECT_CALL(
      *the_mock_client, sendRequest(
        An<const drogon::HttpRequestPtr &>(),   // We use An<> to choose between two overloads
        An<drogon::HttpReqCallback &&>(),
        An<double>()
      )
    )
    // .Times(AnyNumber());
    .WillRepeatedly(
      [](
        const drogon::HttpRequestPtr &req,
        const drogon::HttpReqCallback &&in_callback,
        double timeout
      ) {

        auto the_response = drogon::HttpResponse::newHttpResponse();
        in_callback(drogon::ReqResult::Ok, the_response);
      }
    );


    EXPECT_CALL(
      *the_backend, Fetch(_,_,_)
    )
    .WillRepeatedly(
      [the_host_string](
        std::function<drogon::HttpClientPtr(const std::string&)> in_http_client_factory,
        std::shared_ptr<Message> & in_message,
        std::function<Expected<void>(std::shared_ptr<Message>&)> did_fetch_callback
      ) {
        
        // A simplified version of SingleHostBackend::Fetch() without thread-local storage

        Expected<void> the_result;
        auto the_client_ptr = in_http_client_factory(the_host_string);
        auto the_request = in_message->Request();
        the_request->setPassThrough( true);

        the_client_ptr->sendRequest(
          the_request,
          [did_fetch_callback, &in_message] (
            drogon::ReqResult in_fetch_result,
            const drogon::HttpResponsePtr& in_response
          ) {
            
            in_message->parameter(ParamID::kFetchStatus) = in_fetch_result;
            
            auto the_response = in_response;
            if( in_fetch_result != drogon::ReqResult::Ok ) {
              the_response = drogon::HttpResponse::newHttpResponse();
              the_response->setStatusCode(drogon::HttpStatusCode::k500InternalServerError);
            }

            in_message->SetResponse(the_response);
            auto the_result = did_fetch_callback( in_message);
          }
        );

        return the_result;
      }
    );
      

    auto the_result = the_router->Route(
      the_request, 
      [](const drogon::HttpResponsePtr &in_response) {
        
        std::cout << "We are inside the drogon preAdvice callback!";
      }
    );
    EXPECT_TRUE(the_result.wasSuccessful());


  } catch( const std::exception& in_error ) {

    std::cout << "Router execution failed!" << in_error.what();
    FAIL();
  }
  
  
}






int main(int argc, char **argv)
{
    //testing::InitGoogleTest(&argc, argv);
    testing::InitGoogleMock(&argc, argv);
    return RUN_ALL_TESTS();
}