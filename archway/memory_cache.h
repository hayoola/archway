/**
 * memory_cache.h
 * 
 * Local in-memory cache, which holds the 'hot-spot' Redis objects
*/


#pragma once


#include <string>
#include <unordered_map>
#include "utils/expected.h"



namespace archway {

  class MemoryCache {

    public:

      MemoryCache();

      Expected<std::string&> Lookup(
        const std::string& in_key
      );


      Expected<void> Store(
        const std::string& in_key,
        const std::string& in_value
      );


    private:

      std::unordered_map<std::string, std::string> store_;
  };
}