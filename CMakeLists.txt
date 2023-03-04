cmake_minimum_required(VERSION 3.16)
include(CMake/Utils.cmake)
evaluate_os()

project(ConvertorJsonToTLV)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

if(${OS_WINDOWS})
	message("Windows Platform detected!")

	add_compile_options(
		$<$<CONFIG:>:/MT>
		$<$<CONFIG:Debug>:/MTd>
		$<$<CONFIG:Release>:/MT>)
elseif(${OS_LINUX})
	message("Linux Platform detected!")
endif()

enable_testing()

add_subdirectory(TLV)
add_subdirectory(JsonToTLV)
add_subdirectory(Tests)
