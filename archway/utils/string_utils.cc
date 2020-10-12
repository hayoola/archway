/**
 * string_utils.h
*/


#include "string_utils.h"


using namespace archway;




std::vector<std::string> StringUtils::SplitString(
  const std::string& in_string,
  char in_delimeter
) {

  std::vector<std::string> the_tokens;
  std::string the_single_token;
  std::istringstream the_token_stream(in_string);

  while( std::getline(the_token_stream, the_single_token, in_delimeter) ) {

    the_tokens.emplace_back( the_single_token);
  }

  return the_tokens;
}



std::string StringUtils::InvertHostName( const std::string& in_host_name) {

  std::string the_inverted_str{""};
  std::stringstream the_stream;

  std::vector<std::string> the_tokens = SplitString(in_host_name, '.');

  for( auto the_r_iter = the_tokens.rbegin(); 
    the_r_iter != the_tokens.rend(); 
    ++the_r_iter 
  ) {

    std::string the_token = *the_r_iter;
    if( the_token == "*" ) continue;

    the_stream << the_token;
    if( *(next(the_r_iter)) != "*" ) {
      the_stream << '.';
    }

  }

  the_inverted_str = the_stream.str();


  return the_inverted_str;
}