#include <gtest/gtest.h>
#include <string>
#include <iostream>
#include "archway_types.h"




using namespace archway;


TEST(Archway_Message_Test, Construction) {

  try {

    drogon::HttpRequestPtr  the_request;
    Message the_message(the_request);
    SUCCEED();

  } catch( const std::exception& in_error ) {

    std::cout << "Message construction failed!" << in_error.what();
    FAIL();

  }
}


TEST(Archway_Message_Test, Set_Get_Int) {

  try {

    drogon::HttpRequestPtr  the_request;
    Message the_message(the_request);
    int the_int_value{16};
    the_message.parameter(ParamID::kBackendIndex) = the_int_value;
    auto the_fetched_any = the_message.parameter(ParamID::kBackendIndex);
    int the_fetched_int_value = std::any_cast<int>(the_fetched_any);
    EXPECT_EQ(the_fetched_int_value , the_int_value);
    //EXPECT_EQ( the_message.StringParam(ParamID::kBackendIndex), "");

  } catch( const std::exception& in_error ) {

    std::cout << "Message construction failed! error: " << in_error.what() ;
    FAIL();

  }

}


TEST(Archway_Message_Test, Set_Get_Stage) {

  try {

    drogon::HttpRequestPtr  the_request;
    Message the_message(the_request);
    Stage the_stage{Stage::kEnd};
    the_message.parameter(ParamID::kStage) = the_stage;
    auto the_fetched_any = the_message.parameter(ParamID::kStage);
    Stage the_fetched_stage = std::any_cast<Stage>(the_fetched_any);
    //EXPECT_EQ( the_message.StringParam(ParamID::kStage), "");
    EXPECT_EQ( the_fetched_stage, the_stage);

  } catch( const std::exception& in_error ) {

    std::cout << "Message construction failed!";
    FAIL();

  }

}



int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}