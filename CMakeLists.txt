cmake_minimum_required(VERSION 2.8.9)
project(dots_client C)

# Check for linux
if(UNIX AND NOT APPLE)
    set(LINUX TRUE)
endif()

# Libraries
if(APPLE)
    include_directories(/usr/local/include)
    link_directories(/usr/local/lib)
endif()

if(LINUX)
    link_directories(/usr/lib/x86_64-linux-gnu /usr/local/lib)
endif()

 

# Find all the source file
file(GLOB SOURCES "src/*.c" "src/*.h")
 
add_executable(dots_client ${SOURCES})
target_link_libraries(dots_client coap-2-openssl pthread cbor)
