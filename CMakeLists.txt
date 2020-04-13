cmake_minimum_required(VERSION 2.8.9)
project(dots_client C)
 
# Libraries
include_directories(/usr/local/include)
link_directories(/usr/local/lib)

# Find all the source file
file(GLOB SOURCES "src/*.c" "src/*.h")
 
add_executable(dots_client ${SOURCES})
target_link_libraries(dots_client coap-2-openssl pthread cbor)
