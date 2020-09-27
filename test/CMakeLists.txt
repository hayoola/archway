message(STATUS "Configuring Archway Tests")

add_executable(skeleton_test_unittest skeleton_test.cpp)


set(UNITTEST_TARGETS
  skeleton_test_unittest
)

# Since archway NodeProcessor objects register themselves durng
#   the static init time, we need to compile them directly
#   to the test targets
set( ARCHWAY_NODE_PROCESSORS_SRC
 ${CMAKE_CURRENT_SOURCE_DIR}/../archway/node_processor.cc
 ${CMAKE_CURRENT_SOURCE_DIR}/../archway/root_node_processor.cc
 ${CMAKE_CURRENT_SOURCE_DIR}/../archway/declarations_node_processor.cc
 ${CMAKE_CURRENT_SOURCE_DIR}/../archway/routines_node_processor.cc
)
#message(STATUS "Test Archway files: " ${ARCHWAY_NODE_PROCESSORS_SRC})



set_property(TARGET ${UNITTEST_TARGETS}
             PROPERTY CXX_STANDARD ${DROGON_CXX_STANDARD})
set_property(TARGET ${UNITTEST_TARGETS} PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET ${UNITTEST_TARGETS} PROPERTY CXX_EXTENSIONS OFF)


include(GoogleTest)
foreach(T ${UNITTEST_TARGETS})
  target_sources(${T} PUBLIC ${ARCHWAY_NODE_PROCESSORS_SRC})
  target_link_libraries(${T} PRIVATE archway_lib yaml-cpp drogon GTest::GTest)
  #target_link_libraries(${T} PRIVATE yaml-cpp drogon GTest::GTest)
  target_include_directories(${T} PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/../archway)
  gtest_discover_tests(${T})
endforeach()