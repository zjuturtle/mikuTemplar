project(mikuTemplar)
cmake_minimum_required(VERSION 3.22)

set(CMAKE_CXX_STANDARD 20)

# TODO: better OR tools intergration? Check https://github.com/or-tools/cmake_or-tools
if(UNIX)
  option(BUILD_SHARED_LIBS "Build shared libraries(.so)." ON)
else()
	# only support for static build on Windows.
 set(BUILD_SHARED_LIBS OFF)
endif()

if(BUILD_SHARED_LIBS)
  set(CMAKE_POSITION_INDEPENDENT_CODE ON)
endif()

################
##  OR_TOOLS  ##
################
message(STATUS "Installing OR-Tools...")
# Download and install or-tools at configure time
configure_file(
	${CMAKE_CURRENT_SOURCE_DIR}/cmake/ortools.CMakeLists.txt
	${CMAKE_BINARY_DIR}/ortools-download/CMakeLists.txt)
# CMake configure
execute_process(COMMAND ${CMAKE_COMMAND} -G "${CMAKE_GENERATOR}" .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/ortools-download)
if(result)
  message(FATAL_ERROR "CMake step for ortools failed: ${result}")
endif()
# CMake Build/Install
execute_process(COMMAND ${CMAKE_COMMAND} --build .
  RESULT_VARIABLE result
  WORKING_DIRECTORY ${CMAKE_BINARY_DIR}/ortools-download)
if(result)
  message(FATAL_ERROR "Build step for ortools failed: ${result}")
endif()
message(STATUS "Installing OR-Tools...DONE")

list(APPEND CMAKE_PREFIX_PATH ${CMAKE_BINARY_DIR}/deps)
find_package(ortools REQUIRED)

include_directories("include")
add_executable(martin_ext  "src/martin_ext.cpp")
add_executable(martin_sim  "src/martin_sim.cpp")
add_executable(martin_search  "src/martin_search.cpp")
add_executable(martin_sampling  "src/martin_sampling.cpp")
add_executable(playground  "src/playground.cpp")
