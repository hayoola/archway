#include <gtest/gtest.h>
#include <iostream>
#include "host_program.h"


using namespace archway;

struct DummyInstruction {

  archway::Action operator () (const archway::Message& in_message) {
    return Action::kPass;
  }
};




TEST(Archway_HostProgram_Test, Construction) {

  try {

    
    Instruction the_instruction = DummyInstruction();
    std::shared_ptr<Function> the_function_ptr = std::make_shared<Function>();
    the_function_ptr->AddInstruction(the_instruction);
    HostProgram the_host_program;
    the_host_program.SetFunctionForStage(the_function_ptr, Stage::kReceive);

    auto the_stage_func = the_host_program.GetFunctionForStage(Stage::kReceive);
    drogon::HttpRequestPtr  the_request;
    Message the_message(the_request);
    auto the_result = the_stage_func->Run(the_message);

    EXPECT_EQ(the_result, Action::kPass);

  } catch( const std::exception& in_error ) {

    std::cout << "HostProgram construction failed!" << in_error.what();
    FAIL();
  }

}







int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}