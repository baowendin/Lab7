cmake_minimum_required(VERSION 3.8)
project(SocketProtocol)
set(CMAKE_CXX_STANDARD 17)

set(src 
    protocol
    dto
	binary
)

set(src_client

)
add_executable(Client main_client ${src_client})
add_executable(Main main_wtf ${src})
add_executable(Server main_server ${src})
add_executable(Serialize main_serialize_test ${src})
