
add_library(DSPFlow_generic SHARED
add.cc
div.cc
file.cc
mult.cc
socket_client.cc
socket_server.cc
templateArithmetic.cc
)

target_link_libraries(DSPFlow_generic
	DSPLib
	${CMAKE_THREAD_LIBS_INIT}
)
