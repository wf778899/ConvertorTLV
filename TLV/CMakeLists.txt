project(TLV)

set(SRC_LIST
		TLVObject.cpp)

set(HDR_LIST
		TLVObject.h)

add_library(${PROJECT_NAME} STATIC ${SRC_LIST} ${HDR_LIST})

target_include_directories(${PROJECT_NAME} INTERFACE ${CMAKE_CURRENT_SOURCE_DIR})

source_group("Sources" FILES ${SRC_LIST})
source_group("Headers" FILES ${HDR_LIST})