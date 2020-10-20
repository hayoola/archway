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
    the_message.SetParam(ParamID::kBackendIndex, the_int_value);
    EXPECT_EQ( the_message.IntParam(ParamID::kBackendIndex), the_int_value);
    EXPECT_EQ( the_message.StringParam(ParamID::kBackendIndex), "");

  } catch( const std::exception& in_error ) {

    std::cout << "Message construction failed!";
    FAIL();

  }

}


TEST(Archway_Message_Test, Set_Get_String) {

  try {

    drogon::HttpRequestPtr  the_request;
    Message the_message(the_request);
    Stage the_stage{Stage::kEnd};
    the_message.SetParam(ParamID::kStage, static_cast<int>(the_stage));
    EXPECT_EQ( the_message.StringParam(ParamID::kStage), "");
    EXPECT_EQ( the_message.IntParam(ParamID::kStage), static_cast<int>(the_stage));

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