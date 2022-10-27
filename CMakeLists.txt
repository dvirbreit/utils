
cmake_minimum_required(VERSION 3.17)

project ("develop" VERSION 22.07.01)

add_library(${PROJECT_NAME} INTERFACE)
target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_LIST_DIR}/src)

get_directory_property(hasParent PARENT_DIRECTORY)
if(hasParent)
    message(STATUS "DDSWrapper fetched, tests not generated")
else()
#################### General ####################

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_THREAD_PREFER_PTHREAD TRUE)
find_package(Threads REQUIRED)

##################### RTI-DDS #####################
#[[
set(CONNEXTDDS_DIR /opt/rti_connext_dds-6.0.1)
list(APPEND CMAKE_MODULE_PATH "${CONNEXTDDS_DIR}/resource/cmake")

find_package(RTIConnextDDS "6.0.1" REQUIRED COMPONENTS core)

include_directories(${CONNEXTDDS_DIR}/include)
include_directories(${CONNEXTDDS_DIR}/include/ndds)
include_directories(${CONNEXTDDS_DIR}/include/ndds/hpp)
link_directories(${CONNEXTDDS_DIR}/lib/x64Linux4gcc7.3.0)
]]



include(unit_tests/CMakeLists.txt)

endif()

