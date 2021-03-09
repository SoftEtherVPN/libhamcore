cmake_minimum_required(VERSION 3.11)

project(libhamcore LANGUAGES C)

include(TestBigEndian)

add_library(libhamcore STATIC)

test_big_endian(BIG_ENDIAN)
if(BIG_ENDIAN)
  target_compile_definitions(libhamcore PRIVATE "BYTE_ORDER_BIG_ENDIAN")
endif()

target_include_directories(libhamcore PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

target_sources(libhamcore
  PRIVATE
    FileSystem.c
    FileSystem.h
    Hamcore.c
    Memory.c
    Memory.h
  PUBLIC
    ${CMAKE_CURRENT_SOURCE_DIR}/Hamcore.h
)

find_package(ZLIB REQUIRED)
target_link_libraries(libhamcore PRIVATE ZLIB::ZLIB)
