project ( hyperlink )

cmake_minimum_required( VERSION 2.8 )

set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")

add_executable(sayhello src/main.cpp)

install( TARGETS sayhello RUNTIME DESTINATION /usr/bin )
