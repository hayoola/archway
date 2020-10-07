#include <gtest/gtest.h>
#include <string>
#include <map>
#include <iostream>
#include "../archway/archway_types.h"
#include "../archway/utils/is_valid.h"
#include "../archway/node_processor.h"
#include "../archway/root_node_processor.h"
#include "../archway/archway.h"
#include "../archway/compiler.h"


auto HasStaticYName = archway::is_valid([](auto t) -> decltype( decltype(t)::type::YName()) {return 0;});



auto HasYamlNodeName = archway::is_valid([](auto&& x) -> decltype(x.YamlNodeName()) { return 0;});

template <class T> auto YamlNodeName(T& obj) 
  -> typename std::enable_if<decltype(HasYamlNodeName(obj))::value, std::string>::type {
    
  return obj.YamlNodeName();
}


template <class T> auto YamlNodeName(T& obj) 
  -> typename std::enable_if<!decltype(HasYamlNodeName(obj))::value, std::string>::type {
    
  return "No YamlNodeName";
}


TEST(Archway_Skeleton_Test, Enumeration) {

  EXPECT_EQ( static_cast<int>(archway::Stage::kReceive), 1);
}



TEST(Archway_Skeleton_Test, NodeProcessor) {

  //auto the_name = archway::RootNodeProcessor::Name();
  

  auto the_map = archway::NodeProcessorRegistry::getMap();
  EXPECT_NE( the_map.size(), 0);

  EXPECT_EQ( archway::NodeProcessorRegistry::NewInstance("_root"), nullptr);
  EXPECT_NE( archway::NodeProcessorRegistry::NewInstance("_root_"), nullptr);
}

TEST(Archway_Skeleton_Test, Compiler) {

  const std::string the_yaml = R"(
    confVersion: core/v1alpha1
    declarations:
    - backend:
        name: baseOne
        kind: singleHost
        hosts:
        - address: 127.0.0.1:8088
    routines:
    - hostProgram:
        hosts: [www.example.com, example.com]
        functions:
          - stage: receiveRequest
            instructions:
            - setBackend: baseOne
  )";

  archway::Archway  the_archway;
  archway::Compiler the_compiler(the_archway);
  int the_result = the_compiler.Compile(the_yaml);
  
  EXPECT_EQ(the_result, 0);

}


TEST(Archway_Skeleton_Test, wildcard_playground)  {

  const std::map<std::string, int> the_map{
    {"com.sakura", 1},
    {"com.idea", 2},
    {"com.hayoola.boom", 3}
  };

  
  int the_value{-1};
  std::string the_key{""};
  auto the_key_to_search{"com.hayoola.boom.host1"};
  auto the_lower_iter = the_map.lower_bound(the_key_to_search);
  
  if( the_lower_iter == the_map.end()) {

    
    the_key = the_map.rbegin()->first;
    the_value = the_map.rbegin()->second;

  } else {

    auto the_iter = the_lower_iter;
    the_iter--;
    the_key = the_iter->first;
    the_value = the_iter->second;

  }

  //TODO: We should ensure the found key is indeed a prefix of
  //TODO:   the key to find!

  std::cout << "\nThe key to search: " << the_key_to_search <<
    "\nThe candidate key: " << the_key << 
    " The candidate value: " << the_value << "\n"
  ;
  
  
  
  
}


TEST(Archway_Skeleton_Test, is_valid) {

  // This object has implemented the 'YamlNodeName()' method, so
  //  when we call the global templeted function YamlNodeName()
  //  on it, the call will be forwarded to the function member
  struct MyOK {   

    std::string YamlNodeName() const {
      
      return "Instruction";
    }

    static std::string YName() {
      return "Hayoola";
    }
  };

  // This object hasn't implemented the 'YamlNodeName()' method, so
  //  when we call the global templeted function YamlNodeName()
  //  on it, then a version of that function which returns "No ..."
  //  get enabled by the compiler and called
  struct MyNotOK {};  

  MyOK the_ok;
  auto the_result = YamlNodeName(the_ok);
  EXPECT_EQ(the_result, "Instruction");

  
  auto has_static = HasStaticYName(archway::type_c<MyOK>);
  EXPECT_TRUE(has_static);

  MyNotOK the_not_ok;
  the_result = YamlNodeName(the_not_ok);
  EXPECT_EQ(the_result, "No YamlNodeName");

}


int main(int argc, char **argv)
{
    testing::InitGoogleTest(&argc, argv);
    return RUN_ALL_TESTS();
}