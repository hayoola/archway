#include <gtest/gtest.h>
#include <iostream>
#include <exception>
#include "host_program.h"
#include "utils/expected.h"


using namespace archway;

struct DummyInstruction {

  Expected<void> operator () ( Message& in_message) {
    return {};
  }
};


struct FailingInstruction {

  Expected<void> operator () ( Message& in_message) {
    return std::logic_error("Fatal error! Move away ASAP!");
  }
};




TEST(Archway_HostProgram_Test, Construction_Dynamic) {

  try {

    
    Instruction the_instruction = DummyInstruction();
    std::shared_ptr<DynamicFunction> the_function_ptr = std::make_shared<DynamicFunction>();
    the_function_ptr->AddInstruction(the_instruction);
    HostProgram the_host_program;
    the_host_program.SetFunctionForStage(the_function_ptr, Stage::kReceive);

    auto the_stage_func = the_host_program.GetDynamicFunctionForStage(Stage::kReceive);
    if( ! the_stage_func ) {
      FAIL();
    
    } else {

      drogon::HttpRequestPtr  the_request;
      Message the_message(the_request);
      auto the_result = the_stage_func->Run(the_message);

      EXPECT_TRUE(the_result.wasSuccessful());

    }
    

  } catch( const std::exception& in_error ) {

    std::cout << "HostProgram construction failed!" << in_error.what();
    FAIL();
  }

}




TEST(Archway_HostProgram_Test, Construction_Dynamic_Failing) {

  try {

    
    Instruction the_instruction = FailingInstruction();
    std::shared_ptr<DynamicFunction> the_function_ptr = std::make_shared<DynamicFunction>();
    the_function_ptr->AddInstruction(the_instruction);
    HostProgram the_host_program;
    the_host_program.SetFunctionForStage(the_function_ptr, Stage::kReceive);

    auto the_stage_func = the_host_program.GetDynamicFunctionForStage(Stage::kReceive);
    if( ! the_stage_func ) {
      FAIL();
    
    } else {

      drogon::HttpRequestPtr  the_request;
      Message the_message(the_request);
      auto the_result = the_stage_func->Run(the_message);

      EXPECT_TRUE(!the_result.wasSuccessful());

      std::logic_error the_logic_err("This is a logic error!");
      std::cout << "\nThe error: " << the_logic_err.what() << "\n";
      
      
    }
    

  } catch( const std::exception& in_error ) {

    std::cout << "HostProgram construction failed!" << in_error.what();
    FAIL();
  }

}



TEST(Archway_HostProgram_Test, Construction_Static) {

  try {

    HostProgram the_host_program;
    the_host_program.SetFunctionForStage(
      [] (Message& ) -> Expected<void>{
        return {};
      },
      Stage::kReceive
    );

    
    auto the_stage_function = the_host_program.GetStaticFunctionForStage(Stage::kReceive);
    if( ! the_stage_function ) {

      FAIL();

    } else {

      drogon::HttpRequestPtr  the_request;
      Message the_message(the_request);
      auto the_result = the_stage_function( the_message);
      EXPECT_TRUE(the_result.wasSuccessful());
    }


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