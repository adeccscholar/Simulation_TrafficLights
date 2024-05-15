#central cmake settings for using boost in the project

#if(CMAKE_SYSTEM_PROCESSOR STREQUAL "aarch64")
#   set(BOOST_ROOT "path/to/boost_1_84_0")
#else()
#   set(BOOST_ROOT "path/to/boost_1_84_0")
#endif()

if(WIN32)
   set(BOOST_ROOT "D:\\local\\boost_1_84_0")
endif()

find_package(Boost 1.84 REQUIRED COMPONENTS system)

if(Boost_FOUND)
    message(STATUS "Boost found!")
    message(STATUS "Boost_INCLUDE_DIRS: ${Boost_INCLUDE_DIRS}")
    message(STATUS "Boost_LIBRARIES: ${Boost_LIBRARIES}")

    include_directories(${Boost_INCLUDE_DIRS})
else()
    message(FATAL_ERROR "Boost not found!")
endif()

