#set (CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
#set (CMAKE_LINKER_FLAGS_DEBUG "${CMAKE_LINKER_FLAGS_DEBUG} -fno-omit-frame-pointer -fsanitize=address")
set (CMAKE_CXX_FLAGS_RELEASE " -O3")

add_library(audioengine SHARED AudioGraph.cpp Module.cpp Log.cpp)
add_executable(engineTest test.cpp)
target_link_libraries(engineTest audioengine)
