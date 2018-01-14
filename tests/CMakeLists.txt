cmake_minimum_required(VERSION 3.1)

project(mgstringref_test)

if (CMAKE_CXX_COMPILER_ID MATCHES "Clang|GNU")
    foreach(flag_var
            CMAKE_CXX_FLAGS_RELEASE CMAKE_CXX_FLAGS_RELWITHDEBINFO
            CMAKE_C_FLAGS_RELEASE CMAKE_C_FLAGS_RELWITHDEBINFO)
        string(REGEX REPLACE "-O[^\\s]?" "" ${flag_var} ${${flag_var}})
        set(${flag_var} "${${flag_var}} -O3 -Wall -Wextra -Werror")
    endforeach(flag_var)
endif()

enable_testing()

include(ExternalProject)

ExternalProject_Add(googletest
    GIT_REPOSITORY https://github.com/google/googletest.git
    UPDATE_DISCONNECTED 1
    CMAKE_ARGS
    -DCMAKE_BUILD_TYPE=$<CONFIG>
    -Dgtest_force_shared_crt=ON
    -DCMAKE_DEBUG_POSTFIX=
    PREFIX "${CMAKE_CURRENT_BINARY_DIR}/gtest"
    INSTALL_COMMAND ""
    )

ExternalProject_Get_Property(googletest SOURCE_DIR)
set(GTEST_INCLUDE_DIRS ${SOURCE_DIR}/googletest/include)
include_directories(${GTEST_INCLUDE_DIRS})

ExternalProject_Get_Property(googletest BINARY_DIR)
set(GTEST_LIBS_DIR ${BINARY_DIR}/googlemock/gtest)

add_executable(${PROJECT_NAME}
    mgstringref_test.h
    mgstringref_test_main.cpp
    mgstringref_test_constructors.cpp
    mgstringref_test_comparison.cpp
    mgstringref_test_assign.cpp
)
set_target_properties(${PROJECT_NAME} PROPERTIES
    CXX_STANDARD 11
    CXX_STANDARD_REQUIRED ON
)
add_dependencies(${PROJECT_NAME} googletest)
add_definitions(-DGTEST_INVOKED)

target_link_libraries(${PROJECT_NAME}
    ${GTEST_LIBS_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}gtest${CMAKE_STATIC_LIBRARY_SUFFIX}
    ${GTEST_LIBS_DIR}/${CMAKE_STATIC_LIBRARY_PREFIX}gtest_main${CMAKE_STATIC_LIBRARY_SUFFIX}
)
if(UNIX)
    target_link_libraries(${PROJECT_NAME} pthread)
endif()

add_test(${PROJECT_NAME} ${PROJECT_NAME})
target_link_libraries(${PROJECT_NAME} mgstringref)
