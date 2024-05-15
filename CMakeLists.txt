# CMakeList.txt: CMake-Projekt für "Simulation". Schließen Sie die Quelle ein, und definieren Sie
# projektspezifische Logik hier.
#
cmake_minimum_required(VERSION 3.8)
project(Simulation LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON) 

include(adecc_helper.cmake)

set(OUTPUT_DIR "${CMAKE_SOURCE_DIR}/out/${PLATFORM}/${CMAKE_BUILD_TYPE}")
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${OUTPUT_DIR})
#${CMAKE_BINARY_DIR}

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${OUTPUT_DIR})
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${OUTPUT_DIR})

# Setze den Build-Typ (Debug oder Release)
if(NOT CMAKE_BUILD_TYPE)
  set(CMAKE_BUILD_TYPE Release)
endif()

# Einstellungen für den Debug-Modus

if(WIN32)
   set(CMAKE_GENERATOR_VERSION 20)
   if(CMAKE_BUILD_TYPE STREQUAL Debug)
      set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} /DEBUG /Od")
      set(CMAKE_GENERATOR_PLATFORM x64) # für 64-Bit
      set(CMAKE_GENERATOR_TOOLSET "host=x64")
   endif()
   if(CMAKE_BUILD_TYPE STREQUAL Release)
      set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} /O2 /DNDEBUG")
   endif()
endif()


# Plattform-spezifische Einstellungen für Linux
if(UNIX)
  if(CMAKE_BUILD_TYPE STREQUAL Debug)
    set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -O0 -g")
  endif()

  if(CMAKE_BUILD_TYPE STREQUAL Release)
    set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O3")
  endif()
endif()

set_property(DIRECTORY ${CMAKE_CURRENT_SOURCE_DIR} PROPERTY VS_DEBUGGER_WORKING_DIRECTORY "${OUTPUT_DIR}")

include(boost.cmake)

# Aktivieren Sie Hot Reload für MSVC-Compiler, sofern unterstützt.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()


# Fügen Sie der ausführbaren Datei dieses Projekts eine Quelle hinzu.
add_executable (Simulation "Simulation.cpp" "Simulation.h" 
                "System_Info.cpp" "System_Info.h" 
                "Simulation_Cntrl.cpp" "Simulation_Cntrl.h")

target_link_libraries(Simulation PRIVATE ${Boost_LIBRARIES})
target_include_directories(Simulation PRIVATE ${Boost_INCLUDE_DIRS})
