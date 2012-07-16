add_library(DSPLib
flowBlock.cc
flowGraph.cc
flowPipe.cc
naclDL.cc
tinyxml2.cpp)

add_executable(DSPFlowBin DSPFlowMain.cc)

#Link the DSPFlow binary to the appropriate library files contained in this same directory
target_link_libraries(DSPFlowBin 
	DSPLib 
	${CMAKE_THREAD_LIBS_INIT}
	${DL_LIBRARIES}
)
