message(STATUS "Configuring Archway Tests")


add_executable(
  skeleton_test_unittest skeleton_test.cpp
)

add_executable(
  message_test_unittest message_test.cpp
)


add_executable(
  host_program_test_unittest host_program_test.cpp
)


add_executable(
  router_test_unittest router_test.cpp
)


add_executable(
  redis_test_unittest redis_test.cpp
)


add_executable(
  redis_cache_unittest redis_cache_test.cpp
)



set(UNITTEST_TARGETS
  skeleton_test_unittest
  message_test_unittest
  host_program_test_unittest
  router_test_unittest
  redis_test_unittest
  redis_cache_unittest
)

# Since archway NodeProcessor objects register themselves durng
#   the static init time, we need to compile them directly
#   to the test targets
set( ARCHWAY_NODE_PROCESSORS_SRC
 ${CMAKE_CURRENT_SOURCE_DIR}/../archway/node_processor.cc
 ${CMAKE_CURRENT_SOURCE_DIR}/../archway/root_node_processor.cc
 ${CMAKE_CURRENT_SOURCE_DIR}/../archway/declarations_node_processor.cc
 ${CMAKE_CURRENT_SOURCE_DIR}/../archway/backend_node_processor.cc
 ${CMAKE_CURRENT_SOURCE_DIR}/../archway/routines_node_processor.cc
 ${CMAKE_CURRENT_SOURCE_DIR}/../archway/host_program_node_processor.cc
 ${CMAKE_CURRENT_SOURCE_DIR}/../archway/functions_node_processor.cc
 ${CMAKE_CURRENT_SOURCE_DIR}/../archway/set_backend_node_processor.cc
 ${CMAKE_CURRENT_SOURCE_DIR}/../archway/single_host_backend_node_processor.cc
 ${CMAKE_CURRENT_SOURCE_DIR}/../archway/archway.cc
)
#message(STATUS "Test Archway files: " ${ARCHWAY_NODE_PROCESSORS_SRC})



#set_property(TARGET ${UNITTEST_TARGETS}
#             PROPERTY CXX_STANDARD ${DROGON_CXX_STANDARD})
set_property(TARGET ${UNITTEST_TARGETS} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${UNITTEST_TARGETS} PROPERTY CXX_EXTENSIONS OFF)


include(GoogleTest)
foreach(T ${UNITTEST_TARGETS})
  target_sources(${T} PUBLIC ${ARCHWAY_NODE_PROCESSORS_SRC})
  target_link_libraries(
    ${T} PRIVATE 
    archway_lib 
    yaml-cpp 
    drogon 
    cpp_redis 
    tacopie 
    GTest::GTest 
    ${GMOCK_BOTH_LIBRARIES}
  )
  #target_link_libraries(${T} PRIVATE yaml-cpp drogon GTest::GTest)
  target_include_directories(${T} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../archway)
  target_include_directories(${T} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../third-party)
  gtest_discover_tests(${T})
endforeach()