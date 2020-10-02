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

    private:

      std::vector<std::shared_ptr<Function>> 
        stage_functions_
      ;
  };
}