/**
 * router.h
 * 
*/


#include <string>
#include <unordered_map>
#include <map>
#include "host_program.h"

#pragma once


namespace archway {

  class Router {

    public:

      Router();


      void AddHostProgram(
        const std::vector<std::string>& in_host_names,
        const std::shared_ptr<HostProgram> in_host_program
      );

      std::shared_ptr<HostProgram> FindHostProgram(const std::string& in_host_name);


    private:

      std::unordered_map<std::string, std::shared_ptr<HostProgram>> host_programs_by_exact_names;
      std::map<std::string, std::shared_ptr<HostProgram>> host_programs_by_wildcard;

  };
}