cmake_minimum_required (VERSION 3.5)
project(archway CXX)

include(CheckIncludeFileCXX)

check_include_file_cxx(any HAS_ANY)
check_include_file_cxx(string_view HAS_STRING_VIEW)
if(HAS_ANY AND HAS_STRING_VIEW)
    set(CMAKE_CXX_STANDARD 17)
else()
    set(CMAKE_CXX_STANDARD 14)
endif()

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

add_executable(${PROJECT_NAME} main.cc)



##########
# If you include the drogon source code locally in your project, use this method to add drogon
add_subdirectory(drogon)
target_link_libraries(${PROJECT_NAME} PRIVATE drogon)
##########



find_package(Drogon CONFIG REQUIRED)
target_link_libraries(${PROJECT_NAME} PRIVATE Drogon::Drogon)


# Add yaml-cpp as a libraray
set(YAML_CPP_BUILD_TESTS OFF CACHE BOOL "disable yaml tests")
set(YAML_CPP_BUILD_TOOLS OFF CACHE BOOL "disable yaml tools")
set(YAML_CPP_BUILD_CONTRIB OFF CACHE BOOL "disable yaml contrib")
add_subdirectory(third-party/yaml-cpp/)
target_link_libraries(${PROJECT_NAME} PRIVATE yaml-cpp)


# Add cpp_redis and tacopie libraries
# TODO: Remove tacopie as soon as we develope a trantor-based adaptor for cpp_redis
target_link_libraries(${PROJECT_NAME} PRIVATE cpp_redis tacopie)

# Add a static lib target to be linked with our tests
add_library(${PROJECT_NAME}_lib STATIC main.cc)
target_link_libraries(${PROJECT_NAME}_lib PRIVATE drogon)
target_link_libraries(${PROJECT_NAME}_lib PRIVATE Drogon::Drogon)
target_link_libraries(${PROJECT_NAME}_lib PRIVATE yaml-cpp)




if(CMAKE_CXX_STANDARD LESS 17)
#With C++14, use boost to support any and string_view
    message(STATUS "Using c++14")
    find_package(Boost 1.61.0 REQUIRED)
    target_include_directories(${PROJECT_NAME} PRIVATE ${Boost_INCLUDE_DIRS})
else()
    message(STATUS "Using C++17")
endif()

aux_source_directory(controllers CTL_SRC)
aux_source_directory(filters FILTER_SRC)
aux_source_directory(plugins PLUGIN_SRC)
aux_source_directory(models MODEL_SRC)

## Add archway sources recursively
file(GLOB_RECURSE ARCHWAY_SRC  ${CMAKE_CURRENT_SOURCE_DIR}/archway/*.cc)
#message(STATUS "Archway files: " ${ARCHWAY_SRC})

if(BUILD_TESTING)
  set(CMAKE_MODULE_PATH ${PROJECT_SOURCE_DIR}/cmake_modules/)

  find_package(GTest)
  find_package(GMock)

  if(GTest_FOUND)
    message(STATUS "GTest found")
    enable_testing()
    add_subdirectory(test)

    #### The following line is needed to supress the wierd CTest
    ####  warning about not finding 'DartConfiguration.tcl' file!
    include(CTest)

    if(GMOCK_FOUND)
      message(STATUS "GMock found")
    endif(GMOCK_FOUND)

  endif(GTest_FOUND)

endif(BUILD_TESTING)

file(GLOB_RECURSE SCP_LIST ${CMAKE_CURRENT_SOURCE_DIR}/views/*.csp)
foreach(cspFile ${SCP_LIST})
    message(STATUS "cspFile:" ${cspFile})
    get_filename_component(classname ${cspFile} NAME_WE)
    message(STATUS "view classname:" ${classname})
    ADD_CUSTOM_COMMAND(OUTPUT ${classname}.h ${classname}.cc
            COMMAND drogon_ctl
            ARGS create view ${cspFile}
            DEPENDS ${cspFile}
            VERBATIM )
    set(VIEWSRC ${VIEWSRC} ${classname}.cc)
endforeach()


include_directories(${CMAKE_CURRENT_SOURCE_DIR}/archway)
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/third-party)

target_include_directories(
  ${PROJECT_NAME} PRIVATE 
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/models 
)

# get_property(dirs TARGET ${PROJECT_NAME} PROPERTY INCLUDE_DIRECTORIES)
# foreach(dir ${dirs})
#   message(STATUS "include dir='${dir}'")
# endforeach()

target_sources(
  ${PROJECT_NAME} PRIVATE 
  ${SRC_DIR} 
  ${CTL_SRC} 
  ${FILTER_SRC} 
  ${VIEWSRC} 
  ${PLUGIN_SRC} 
  ${MODEL_SRC} 
  ${ARCHWAY_SRC})
################################
# uncomment the following line for dynamically loading views 
#set_property(TARGET ${PROJECT_NAME} PROPERTY ENABLE_EXPORTS ON)


## Link all source files to the project_lib static lib as well
target_include_directories(
  ${PROJECT_NAME}_lib PRIVATE 
  ${CMAKE_CURRENT_SOURCE_DIR}
  ${CMAKE_CURRENT_SOURCE_DIR}/models
)
target_sources(
  ${PROJECT_NAME}_lib PRIVATE 
  ${SRC_DIR} 
  ${CTL_SRC} 
  ${FILTER_SRC} 
  ${VIEWSRC} 
  ${PLUGIN_SRC} 
  ${MODEL_SRC} 
  ${ARCHWAY_SRC}
)
