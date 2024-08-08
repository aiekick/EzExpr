cmake_minimum_required(VERSION 3.5)
cmake_policy(SET CMP0079 NEW) # for line 67

set(PROJECT EzExpr_Demo)

enable_language(C CXX)
project(${PROJECT} CXX)

## some defines for debug mode (before 3rdparty.cmake)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

## some defines for debug mode (before 3rdparty.cmake)
set(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -D_DEBUG")

set(FINAL_BIN_DIR ${CMAKE_SOURCE_DIR}/bin)
set(CMAKE_PDB_OUTPUT_DIRECTORY "${FINAL_BIN_DIR}/debug")

set(CMAKE_INSTALL_PREFIX .)
set(CMAKE_INSTALL_BINDIR .)

if(NOT CMAKE_DEBUG_POSTFIX)
  set(CMAKE_DEBUG_POSTFIX _debug)
endif()

if (UNIX)
	option(USE_LEAK_SANITIZER "Enable the Leak Sanitizer" OFF)
endif()

set(ORIGINAL_CMAKE_CXX_FLAGS ${CMAKE_CXX_FLAGS})

if (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    add_definitions(-DMSVC)
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} /std:c++11 /Z")
else()
	set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=c++11")
endif()


include(cmake/3rdparty.cmake)

add_definitions(${GLFW_DEFINITIONS})
add_definitions(-D_CRT_SECURE_NO_WARNINGS)
add_definitions(-DIMGUI_IMPL_OPENGL_LOADER_GLAD)

enable_testing()
add_definitions(-DTEST_ENABLED)
add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/EzExpr)

file(GLOB_RECURSE CMAKE_SOURCES ${CMAKE_SOURCE_DIR}/cmake/*.*)
source_group(cmake FILES ${CMAKE_SOURCES})
	
file(GLOB_RECURSE MAIN_SOURCES ${CMAKE_SOURCE_DIR}/src/*.*)
source_group(TREE ${CMAKE_SOURCE_DIR}/src PREFIX src FILES ${MAIN_SOURCES})
	
file(GLOB_RECURSE CORE_SOURCES ${CMAKE_SOURCE_DIR}/EzExpr/EzExpr.hpp)
source_group(src\\core\\lib FILES ${CORE_SOURCES})

file(GLOB IMGUI_IMPL_SOURCES 
	${CMAKE_SOURCE_DIR}/3rdparty/ImGuiPack/3rdparty/imgui_docking/backends/imgui_impl_glfw.cpp
	${CMAKE_SOURCE_DIR}/3rdparty/ImGuiPack/3rdparty/imgui_docking/backends/imgui_impl_glfw.h
	${CMAKE_SOURCE_DIR}/3rdparty/ImGuiPack/3rdparty/imgui_docking/backends/imgui_impl_opengl3.cpp
	${CMAKE_SOURCE_DIR}/3rdparty/ImGuiPack/3rdparty/imgui_docking/backends/imgui_impl_opengl3.h)
source_group(src\\ImGuiImpl FILES ${IMGUI_IMPL_SOURCES})

if (APPLE)
	##set(ICON icon.icns)
	##set_source_files_properties(icon.icns PROPERTIES MACOSX_PACKAGE_LOCATION "Resources")
endif()

include_directories(
	${CMAKE_SOURCE_DIR}
	${CMAKE_SOURCE_DIR}/3rdparty
	${CMAKE_SOURCE_DIR}/src
	${GLFW_INCLUDE_DIR}
	${GLAD_INCLUDE_DIR}
	${OPENGL_INCLUDE_DIR}
	${IMGUIPACK_INCLUDE_DIRS})

if (USE_LEAK_SANITIZER)
	add_compile_options(-fexceptions -pthread -ldl -lGL -Wall -g -fsanitize=undefined -fno-sanitize-recover -fsanitize=float-cast-overflow -fsanitize=leak -fsanitize=address -fsanitize-address-use-after-scope -fstack-protector -fstack-protector-all)
	add_link_options(-fexceptions -pthread -ldl -lGL -Wall -g -fsanitize=undefined -fno-sanitize-recover -fsanitize=float-cast-overflow -fsanitize=leak -fsanitize=address -fsanitize-address-use-after-scope -fstack-protector -fstack-protector-all)
endif()

set(SOURCES 
	${MAIN_SOURCES}
	${CORE_SOURCES}
	${CMAKE_SOURCES}
	${IMGUI_IMPL_SOURCES}
)

add_executable(${PROJECT} MACOSX_BUNDLE ${SOURCES})

set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_INSTALL_PREFIX "${CMAKE_SOURCE_DIR}")
install(TARGETS ${PROJECT} DESTINATION ${CMAKE_INSTALL_PREFIX})

if(UNIX)
    target_compile_options(${PROJECT} PUBLIC -Wno-unknown-pragmas)
endif()

if (APPLE)
	set_target_properties(${PROJECT}
		PROPERTIES
		OUTPUT_NAME "${PROJECT}_Macos"
		MACOSX_BUNDLE_EXECUTABLE_NAME "${PROJECT}"
		MACOSX_BUNDLE_BUNDLE_NAME "${PROJECT}"
		FOLDER "${PROJECT}"
		##RESOURCE icon.icns
		MACOSX_BUNDLE_SHORT_VERSION_STRING ""
		MACOSX_BUNDLE_LONG_VERSION_STRING ""
		##MACOSX_BUNDLE_ICON_FILE icon.icns
		MACOSX_BUNDLE_INFO_PLIST "${CMAKE_CURRENT_SOURCE_DIR}/MacOSXBundleInfo.plist.in")
elseif(WIN32)
	set_target_properties(${PROJECT} PROPERTIES OUTPUT_NAME "${PROJECT}_Win32")
elseif(UNIX)
	set_target_properties(${PROJECT} PROPERTIES OUTPUT_NAME "${PROJECT}_Unix")
endif()

set(CMAKE_CXX_STANDARD 11)
set_target_properties(${PROJECT} PROPERTIES CXX_STANDARD 11)
	
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

set(CMAKE_INSTALL_PREFIX "${CMAKE_CURRENT_SOURCE_DIR}")
install(TARGETS ${PROJECT} DESTINATION ${CMAKE_INSTALL_PREFIX})

target_link_libraries(${PROJECT} PRIVATE
	${OPENGL_LIBRARIES}
	${GLFW_LIBRARIES}
	${GLAD_LIBRARIES}
	${IMGUIPACK_LIBRARIES})

set_target_properties(${PROJECT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY "${FINAL_BIN_DIR}")
set_target_properties(${PROJECT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${FINAL_BIN_DIR}")
set_target_properties(${PROJECT} PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${FINAL_BIN_DIR}")
