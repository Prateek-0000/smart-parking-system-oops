cmake_minimum_required(VERSION 3.15)
project(SmartParking CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Find the Crow package (installed by vcpkg)
find_package(Crow REQUIRED)

# Add our server executable
add_executable(parking_server server.cpp)

# Link our server against Crow and its dependency, asio
target_link_libraries(parking_server PRIVATE Crow::Crow asio::asio)