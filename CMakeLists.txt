cmake_minimum_required(VERSION 3.8)
project(SocketProtocol)
set(CMAKE_CXX_STANDARD 17)

set(src 
    protocol
    dto
	binary
)

add_executable(Main main_wtf ${src})
add_executable(Server main_server ${src})
add_executable(Serialize main_serialize_test ${src})