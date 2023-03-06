project(JsonToTLV)

set(SRC_LIST
		main.cpp
		Utils.cpp)

set(HDR_LIST
		json.hpp
		Utils.h)

add_executable(${PROJECT_NAME} ${SRC_LIST} ${HDR_LIST})

target_link_libraries(${PROJECT_NAME} TLV)

