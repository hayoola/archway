/**
 * router.h
 * 
*/


#include <string>
#include <unordered_map>
#include <map>
#include "host_program.h"
#include "utils/expected.h"

#pragma once


namespace archway {

  class Router : public std::enable_shared_from_this<Router> {

    public:

      Router();


      void AddHostProgram(
        const std::vector<std::string>& in_host_names,
        const std::shared_ptr<HostProgram> in_host_program
      );

      std::shared_ptr<HostProgram> FindHostProgram(const std::string& in_host_name);

      Expected<void> Route(const drogon::HttpRequestPtr& in_request);

      Expected<void> MoveToNextState(std::shared_ptr<Message>& in_message);


    private:

      std::unordered_map<std::string, std::shared_ptr<HostProgram>> host_programs_by_exact_names;
      std::map<std::string, std::shared_ptr<HostProgram>> host_programs_by_wildcard;
      std::shared_ptr<HostProgram>  default_host_program_;
      HostProgram build_in_stage_functions_;
      
      /*
      std::array<
        std::array<Stage, static_cast<size_t>(Stage::_kLast)>,
        static_cast<size_t>(Action::_kLast)
      > state_machine_;
      */


  };
}