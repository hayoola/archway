/**
 *  host_program.h 
*/


#pragma once


#include <vector>
#include "archway_types.h"
#include "dynamic_function.h"
#include "utils/expected.h"


namespace archway {

  /**
   * We want to put both `DynamicFunctions` and build-in `Router`
   *  functions, which drive the main request-processing job.
   * Actually we want to re-use HostProgram as the main data structure
   *  of the `Router` for storing static function pointers corresponding to
   *  the different stages of the processing job, governed by the 
   *  state machine inside the router.
  */
  using FunctionPtr = std::variant<
    std::shared_ptr<DynamicFunction>,
    std::function<Expected<void>(Message&)>
  >;

  class HostProgram {

    public:

      HostProgram() {

        for( auto the_function_ptr : stage_functions_) {
          the_function_ptr = std::shared_ptr<DynamicFunction>(nullptr);
        }
      }



      HostProgram(
        const std::array<
          std::function<Expected<void>(Message&)>, 
          static_cast<size_t>(Stage::_kLast)
        >& in_functions
      ) {

        for( size_t i; i < in_functions.size(); i++ ) {
          stage_functions_[i] = in_functions[i];
        }
      }

      
      /**
       * The DynamicFunction version
      */
      void SetFunctionForStage(
        const std::shared_ptr<DynamicFunction> in_function,
        const Stage in_stage
      ) {
        stage_functions_[static_cast<size_t>(in_stage)] = in_function;
      }

      
      /**
       * The static build-in function version
      */
      void SetFunctionForStage(
        const std::function<Expected<void>(Message&)> in_function,
        const Stage in_stage
      ) {
        stage_functions_[static_cast<size_t>(in_stage)] = in_function;
      }

      
      std::shared_ptr<DynamicFunction> 
      GetDynamicFunctionForStage(const Stage in_stage) {
        
        std::shared_ptr<DynamicFunction> the_result{nullptr};

        auto the_generic_ptr = stage_functions_[static_cast<size_t>(in_stage)];
        auto the_dynamic_func_ptr = 
          std::get_if< std::shared_ptr<DynamicFunction> >(&the_generic_ptr)
        ;
        
        if( the_dynamic_func_ptr ) {
          the_result = *the_dynamic_func_ptr;
        }

        return the_result;
      }



      std::function<Expected<void>(Message&)> 
      GetStaticFunctionForStage(const Stage in_stage) {
        
        std::function<Expected<void>(Message&)> the_result{nullptr};

        auto the_generic_ptr = stage_functions_[static_cast<size_t>(in_stage)];
        auto the_dynamic_func_ptr = 
          std::get_if< std::function<Expected<void>(Message&)>  >(&the_generic_ptr)
        ;
        
        if( the_dynamic_func_ptr ) {
          the_result = *the_dynamic_func_ptr;
        }

        return the_result;
      }


      static Stage StringToStage( std::string in_string ) {

        Stage the_stage{Stage::kStart};

        const static std::map<std::string, Stage> s_string_to_stage_map {
          {"receiveRequest", Stage::kReceive},
          {"hashRequest", Stage::kHash},
          {"missRequest", Stage::kMiss},
          {"hitRequest", Stage::kHit},
          {"backendFetch", Stage::kBackendFetch},
          {"backendResponse", Stage::kBackendResponse},
          {"backendError", Stage::kBackendError},
          {"deliverResponse", Stage::kDeliver}
        };

        auto the_iter = s_string_to_stage_map.find(in_string);
        if( the_iter != s_string_to_stage_map.end() ) {
          the_stage = the_iter->second;
        }

        return the_stage;
      }

    private:

      std::array<FunctionPtr, static_cast<size_t>(Stage::_kLast)> 
        stage_functions_
      ;
  };
}