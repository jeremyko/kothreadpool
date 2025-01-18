cmake_minimum_required(VERSION 2.8)

project(KoThreadPool)

include_directories(include)

set(CMAKE_CXX_STANDARD 11)

if(WIN32)
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /nologo /W3 /Gy /EHsc")
  set(CMAKE_CXX_FLAGS_DEBUG           " /MTd /ZI /Od ")
  set(CMAKE_CXX_FLAGS_RELEASE         " /MT  /O3 ")
elseif(APPLE OR CMAKE_SYSTEM_NAME MATCHES "Linux")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
  set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")
endif()

if(WIN32)
  add_definitions(/wd4244 /wd4324)
elseif(APPLE OR CMAKE_SYSTEM_NAME MATCHES "Linux")
  add_definitions(-Wall)
endif()

add_subdirectory(sample)

