
cmake_minimum_required(VERSION 3.17)

project(dds_wrapper_unit_tests)


#[[
add_library(test_files
    ${CMAKE_CURRENT_LIST_DIR}/test_types/test_special_types.cxx
    ${CMAKE_CURRENT_LIST_DIR}/test_types/test_special_typesPlugin.cxx
    ${CMAKE_CURRENT_LIST_DIR}/../pingpong-dds_demo/type_files/pingpong.cxx
    ${CMAKE_CURRENT_LIST_DIR}/../pingpong-dds_demo/type_files/pingpongPlugin.cxx
)

target_include_directories(test_files PUBLIC
    ${CMAKE_CURRENT_LIST_DIR}/
    ${CMAKE_CURRENT_LIST_DIR}/../pingpong-dds_demo/type_files
)


target_link_libraries(test_files PUBLIC
    RTIConnextDDS::cpp2_api
    ${dds_zero_copy}
    Threads::Threads
    dds_wrapper
)]]

add_executable(${PROJECT_NAME}
    ${CMAKE_CURRENT_LIST_DIR}/dds_wrapper_test.cpp
    #${CMAKE_CURRENT_LIST_DIR}/zero_copy_test.cpp
)


target_link_libraries(${PROJECT_NAME} PRIVATE
    #RTIConnextDDS::cpp2_api
    Threads::Threads
    gtest
    gtest_main
    develop
    #test_files
)
#[[
add_subdirectory(${CMAKE_CURRENT_LIST_DIR}/ipc_tests)

########### Copying files to binary dir ###############

add_custom_command(
    TARGET
        ${PROJECT_NAME} POST_BUILD
    COMMAND
        ${CMAKE_COMMAND} -E copy_if_different
            "/workspaces/rti_license.dat"
            "${CMAKE_CURRENT_LIST_DIR}/unit_tests_config.xml"
            "${CMAKE_CURRENT_BINARY_DIR}"


    COMMENT "Copying external files to binary dir"
    VERBATIM
)]]