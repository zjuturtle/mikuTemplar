project(mikuTemplar)
cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 17)

if(POLICY CMP0022)
	cmake_policy(SET CMP0022 NEW)
endif()

# option() honors normal variables.
if(POLICY CMP0077)
	cmake_policy(SET CMP0077 NEW)
endif()

# Set default build type to Release if empty.
set(CMAKE_BUILD_TYPE "Release" CACHE STRING
	"Choose the type of build, options are: Debug Release RelWithDebInfo MinSizeRel.")

project(CMakeORTools VERSION 1.0 LANGUAGES CXX)

include(CTest)
include(GNUInstallDirs)

if(UNIX)
  option(BUILD_SHARED_LIBS "Build shared libraries(.so)." ON)
else()
	# only support for static build on Windows.
 set(BUILD_SHARED_LIBS OFF)
endif()

# Build Dependencies
add_subdirectory(cmake)
include_directories("include")
add_executable(martin_ext  "src/martin_ext.cpp")
add_executable(martin_sim  "src/martin_sim.cpp")
add_executable(martin_search  "src/martin_search.cpp")
add_executable(martin_sampling  "src/martin_sampling.cpp")
add_executable(playground  "src/playground.cpp")

target_link_libraries(martin_ext PRIVATE ortools::ortools)
target_link_libraries(martin_sim PRIVATE ortools::ortools)
target_link_libraries(martin_search PRIVATE ortools::ortools)
target_link_libraries(martin_sampling PRIVATE ortools::ortools)
target_link_libraries(playground PRIVATE ortools::ortools)