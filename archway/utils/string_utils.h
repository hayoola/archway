/**
 * string_utils.h
 * 
 * Various string manipulation utilities mainly created for processing
 *  the wildcard hostname matching
*/


#pragma once


#include <string>
#include <vector>
#include <sstream>


namespace archway {

  
  struct StringUtils {

    /**
     * Get the next iterator
     * !Waring: Always first check for iter != Object.end()
     * Borrowed from boost!
    */
    template <typename Iter>
    static Iter next(Iter iter)
    {
        return ++iter;
    }

    /**
     * Utility function for splitting a string based on an
     *  arbitrary delimeter.
     * Ref: https://www.fluentcpp.com/2017/04/21/how-to-split-a-string-in-c/
    */
    static std::vector<std::string> SplitString(
      const std::string& in_string,
      char in_delimeter
    );


    static std::string InvertHostName( const std::string& in_host_name);
  
  };
  
  

  



  
  
}