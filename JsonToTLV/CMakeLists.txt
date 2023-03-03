project(JsonToTLV)

set(SRC_LIST
		main.cpp)

set(HDR_LIST
		json.hpp)

add_executable(${PROJECT_NAME} ${SRC_LIST} ${HDR_LIST})

target_link_libraries(${PROJECT_NAME} TLV)

