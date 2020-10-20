#include <gtest/gtest.h>
#include <iostream>
#include <exception>
#include "host_program.h"
#include "utils/expected.h"


using namespace archway;

static const std::string s_error_string{"Fatal error! Move away ASAP!"};

struct DummyInstruction {

  Expected<void> operator () ( Message& in_message) {
    return {};
  }
};


struct FailingInstruction {

  Expected<void> operator () ( Message& in_message) {
    return std::logic_error(s_error_string);
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
      EXPECT_EQ(the_result.error(), s_error_string);

      //std::logic_error the_logic_err("This is a logic error!");
      //std::cout << "\nThe error: " << the_result.error() << "\n";
      
      Expected<int> the_int_expected(-657);
      EXPECT_TRUE(the_int_expected.isValid());
      EXPECT_EQ(the_int_expected.get(), -657);

      const std::string the_error_string{"A typical logic error"};
      std::logic_error the_hayoola(the_error_string);
      Expected<int> the_failing_expected(the_hayoola);
      EXPECT_FALSE(the_failing_expected.isValid());
      EXPECT_EQ(the_failing_expected.error(), the_error_string);
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