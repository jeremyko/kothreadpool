cmake_minimum_required (VERSION 2.8) 

project (KoThreadPool)

include_directories(include)

SET(CMAKE_CXX_STANDARD 11) 

if(WIN32)
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /nologo /W3 /Gy /EHsc")
   SET(CMAKE_CXX_FLAGS_DEBUG           " /MTd /ZI /Od ")
   SET(CMAKE_CXX_FLAGS_RELEASE         " /MT  /O3 ")   
elseif(APPLE OR CMAKE_SYSTEM_NAME MATCHES "Linux")   
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -pthread") 
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -Wall")
   SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -g")   
endif()

if(WIN32)    
    add_definitions(/wd4244 /wd4324)
elseif(APPLE OR CMAKE_SYSTEM_NAME MATCHES "Linux")    
    add_definitions(-Wall)
endif()

add_subdirectory (sample)

