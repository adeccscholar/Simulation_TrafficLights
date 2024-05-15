#definition of macros to simplyfy some actions

if(CMAKE_SYSTEM_NAME STREQUAL "Windows")
    set(PLATFORM "Windows")
    add_definitions(-D_WIN32_WINNT=0x0601) # Windows 7
elseif(CMAKE_SYSTEM_NAME STREQUAL "Linux")
    set(PLATFORM "Linux")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Darwin")
    set(PLATFORM "macOS")
elseif(CMAKE_SYSTEM_NAME STREQUAL "Android")
    set(PLATFORM "Android")
endif()

# settings for compiling at the Raspberry with gcc 13.2

if(UNIX AND NOT APPLE)
    set(CMAKE_SYSTEM_PROCESSOR aarch64)
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -march=armv8-a")
    set(CMAKE_C_COMPILER /usr/local/bin/gcc)
    set(CMAKE_CXX_COMPILER /usr/local/bin/g++)
    set(CMAKE_INCLUDE_PATH /usr/local/include/c++/13.2.0)
    set(CMAKE_LIBRARY_PATH /usr/local/lib64)
    set(CMAKE_PREFIX_PATH /usr/local/lib64)
    
    #include_directories("/usr/local/include/c++/13.2.0")
    #link_directories("/usr/local/lib64")
    #include_directories("/pfad/zu/boost/include")
    #include_directories("/pfad/zu/qt/include")
    #link_directories("/pfad/zu/boost/lib")
    #link_directories("/pfad/zu/qt/lib")
endif()



macro(clone_repository REPO_PATH REPO_NAME REPO_URL)
    if(DEFINED ${REPO_PATH})
        if(NOT EXISTS ${${REPO_PATH}})
            message("Cloning ${REPO_NAME} repository...")
            execute_process(
                COMMAND git clone ${REPO_URL} ${${REPO_PATH}}
                RESULT_VARIABLE GIT_CLONE_RESULT
            )
            if(NOT GIT_CLONE_RESULT EQUAL "0")
                message(FATAL_ERROR "Failed to clone ${REPO_NAME} repository. Aborting.")
            endif()
        endif()
    else()
        message(WARNING "${REPO_PATH} is not defined. Skipping repository cloning.")
    endif()
endmacro()

macro(add_include_directory_if_exists TARGET INCLUDE_PATH)
    if(DEFINED ${INCLUDE_PATH})
       if(EXISTS ${${INCLUDE_PATH}})
          target_include_directories(${TARGET} PUBLIC ${${INCLUDE_PATH}})
       else()
          message(WARNING "Include directory ${${INCLUDE_PATH}} does not exist.")
       endif()
    endif()
endmacro()
