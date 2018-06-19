cmake_minimum_required(VERSION 3.9)
project(odu)

set(CMAKE_CXX_STANDARD 14)
find_package(Boost COMPONENTS system thread filesystem REQUIRED)
include_directories(
        ${CMAKE_CURRENT_BINARY_DIR}
        ${CMAKE_CURRENT_SOURCE_DIR}
        ${Boost_INCLUDE_DIRS}
)
#FILE(GLOB SRC_FILES src/*.cpp)
#FILE(GLOB HEADER_FILES include/*.h)

add_executable(odu main.cpp
#        ${SRC_FILES}
#        ${HEADER_FILES}
        )

target_link_libraries(odu
        ${Boost_LIBRARIES}
        )
install(TARGETS odu RUNTIME DESTINATION $ENV{HOME}/bin)
