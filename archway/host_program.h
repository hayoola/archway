/**
 *  host_program.h 
*/


#pragma once


#include <vector>
#include "archway_types.h"
#include "function.h"


namespace archway {

  class HostProgram {

    public:

      HostProgram() {

        stage_functions_.reserve(static_cast<size_t>(Stage::_kLast));
        
        for( int i = 0; i < static_cast<size_t>(Stage::_kLast); i++ ) {
          stage_functions_.emplace_back( nullptr );
        }
        
      }

      void SetFunctionForStage(
        const std::shared_ptr<Function> in_function,
        const Stage in_stage
      ) {
        stage_functions_[static_cast<int>(in_stage)] = in_function;
      }

      std::shared_ptr<Function> GetFunctionForStage(const Stage in_stage) {
        return stage_functions_[static_cast<int>(in_stage)];
      }


      static Stage StringToStage( std::string in_string ) {

        Stage the_stage{Stage::kInvalid};

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

      std::vector<std::shared_ptr<Function>> 
        stage_functions_
      ;
  };
}