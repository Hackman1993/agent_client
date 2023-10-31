cmake_minimum_required(VERSION 3.26)
project(agent_client)
set(CMAKE_CXX_STANDARD 20)

file(GLOB_RECURSE CPP_HEADERS source/*.h source/*.hpp)
file(GLOB_RECURSE CPP_SOURCES source/*.cpp source/*.cc)

find_package(Boost REQUIRED COMPONENTS url thread json fiber filesystem)
find_package(sahara REQUIRED)
find_package(agent REQUIRED)
find_package(Protobuf REQUIRED)

add_executable(${PROJECT_NAME} ${CPP_HEADERS} ${CPP_SOURCES} )
target_include_directories(${PROJECT_NAME} PRIVATE source)

target_link_libraries(${PROJECT_NAME} PRIVATE sahara::sahara agent::agent Boost::filesystem)
target_link_libraries(${PROJECT_NAME} PRIVATE protobuf::libprotoc protobuf::libprotobuf protobuf::libprotobuf-lite)