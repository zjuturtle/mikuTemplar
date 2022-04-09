cmake_minimum_required(VERSION 3.22)
project(mikuTemplar)
set(CMAKE_CXX_STANDARD 17)

include_directories("include")
set(SRC_CORE
    "src/core/martin_simulator.cpp")
add_executable(martin_ext ${SRC_CORE} "src/martin_ext.cpp")
add_executable(martin_sim ${SRC_CORE} "src/martin_sim.cpp")
add_executable(martin_sampling ${SRC_CORE} "src/martin_sampling.cpp")
add_executable(playground ${SRC_CORE} "src/playground.cpp")
