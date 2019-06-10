cmake_minimum_required(VERSION 3.7.2)
project(WolfwaveUpdate)

add_subdirectory("${CMAKE_CURRENT_SOURCE_DIR}/libFirmwareUpdate++" "${CMAKE_CURRENT_BINARY_DIR}/libFirmwareUpdate++")

set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)
set(CMAKE_AUTOUIC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)

find_package(Qt5 COMPONENTS Core Gui Widgets REQUIRED)

if((${CMAKE_SYSTEM_NAME} MATCHES "Windows"))
	set(WolfwaveUpdate_rc "${CMAKE_CURRENT_SOURCE_DIR}/res/wolfwave.rc")
	set(CMAKE_RC_COMPILER_INIT windres)
	enable_language(RC)
	set(CMAKE_RC_COMPILE_OBJECT "<CMAKE_RC_COMPILER> -O coff <DEFINES> <FLAGS> <SOURCE> <OBJECT>")
else()
	set(WolfwaveUpdate_rc "")
endif()

set(WolfwaveUpdate_qrc "${CMAKE_CURRENT_SOURCE_DIR}/res/wolfwave.qrc")

file(GLOB_RECURSE WolfwaveUpdate_src FOLLOW_SYMLINKS "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp")
add_executable(WolfwaveUpdate "${WolfwaveUpdate_src}" "${WolfwaveUpdate_rc}" "${WolfwaveUpdate_qrc}")
target_link_libraries(WolfwaveUpdate PUBLIC Qt5::Core Qt5::Gui Qt5::Widgets)
target_link_libraries(WolfwaveUpdate PRIVATE FirmwareUpdate++)
set_target_properties(WolfwaveUpdate PROPERTIES
    CXX_STANDARD 14
    CXX_STANDARD_REQUIRED ON
)

target_compile_options(WolfwaveUpdate PRIVATE $<$<CXX_COMPILER_ID:GNU>:-O2 -Wall -Wextra>)
