/**
 * compiler.h
 * 
 * The object that compiles the YAML configuration file into
 *  a bunch of data structures residing inside the Archway object
 * 
**/


#pragma once


#include <string>
#include "archway.h"


namespace archway {

  
  class Compiler {

    public:

      Compiler( const Archway& in_archway);

      int Compile(const std::string& in_config_content );


      private:
        const Archway& archway_;
  };
}