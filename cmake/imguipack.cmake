set(IMGUIPACK_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ImGuiPack)
set(IMGUIPACK_LIBRARIES ImGuiPack)

# ON
set(USE_IMPLOT ON CACHE BOOL "" FORCE)
set(USE_IM_TOOLS ON CACHE BOOL "" FORCE)

# OFF
set(USE_IM_NODES OFF CACHE BOOL "" FORCE)
set(USE_IM_GUIZMO OFF CACHE BOOL "" FORCE)
set(USE_IM_COOL_BAR OFF CACHE BOOL "" FORCE)
set(USE_IMGUI_MARKDOW OFF CACHE BOOL "" FORCE)
set(USE_IMGUI_NODE_FLOW OFF CACHE BOOL "" FORCE)
set(USE_IM_GRADIENT_HDR OFF CACHE BOOL "" FORCE)
set(USE_IMGUI_NODE_EDITOR OFF CACHE BOOL "" FORCE)
set(USE_IMGUI_FILE_DIALOG OFF CACHE BOOL "" FORCE)
set(USE_IMGUI_COLOR_TEXT_EDIT OFF CACHE BOOL "" FORCE)
set(IMGUIPACK_USE_STD_FILESYSTEM OFF CACHE BOOL "" FORCE)

add_subdirectory(${CMAKE_CURRENT_SOURCE_DIR}/3rdparty/ImGuiPack)

if(USE_SHARED_LIBS)
	target_compile_definitions(ImGuiPack INTERFACE BUILD_CTOOLS_SHARED_LIBS)	
	set_target_properties(ImGuiPack PROPERTIES FOLDER 3rdparty/aiekick/Shared)
	set_target_properties(freetype PROPERTIES FOLDER 3rdparty/Shared)
	if (USE_IMGUI_FILE_DIALOG)
		set_target_properties(boost_regex PROPERTIES FOLDER 3rdparty/Shared)
	endif()
else()
	set_target_properties(ImGuiPack PROPERTIES FOLDER 3rdparty/aiekick/Static)
	set_target_properties(freetype PROPERTIES FOLDER 3rdparty/Static)
	if (USE_IMGUI_FILE_DIALOG)
		set_target_properties(boost_regex PROPERTIES FOLDER 3rdparty/Static)
	endif()
endif()

set_target_properties(ImGuiPack PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${FINAL_BIN_DIR}")
set_target_properties(ImGuiPack PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${FINAL_BIN_DIR}")
set_target_properties(ImGuiPack PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${FINAL_BIN_DIR}")
set_target_properties(ImGuiPack PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${FINAL_BIN_DIR}")

set_target_properties(freetype PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${FINAL_BIN_DIR}")
set_target_properties(freetype PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${FINAL_BIN_DIR}")
set_target_properties(freetype PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${FINAL_BIN_DIR}")
set_target_properties(freetype PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${FINAL_BIN_DIR}")

if (USE_IMPLOT)
	add_definitions(-DUSE_IMPLOT)
endif()

if (USE_IM_COOL_BAR)
	add_definitions(-DUSE_IM_COOL_BAR)
endif()

if (USE_IM_GRADIENT_HDR)
	add_definitions(-DUSE_IM_GRADIENT_HDR)
endif()

if (USE_IMGUI_FILE_DIALOG)
	add_definitions(-DUSE_IMGUI_FILE_DIALOG)
	set_target_properties(boost_regex PROPERTIES RUNTIME_OUTPUT_DIRECTORY_DEBUG "${FINAL_BIN_DIR}")
	set_target_properties(boost_regex PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELEASE "${FINAL_BIN_DIR}")
	set_target_properties(boost_regex PROPERTIES RUNTIME_OUTPUT_DIRECTORY_MINSIZEREL "${FINAL_BIN_DIR}")
	set_target_properties(boost_regex PROPERTIES RUNTIME_OUTPUT_DIRECTORY_RELWITHDEBINFO "${FINAL_BIN_DIR}")
endif()

if (USE_IM_GUIZMO)
	add_definitions(-DUSE_IM_GUIZMO)
endif()

if (USE_IM_TOOLS)
	add_definitions(-DUSE_IM_TOOLS)
endif()

if (USE_IM_NODES)
	add_definitions(-DUSE_IM_NODES)
endif()

if (USE_IMGUI_NODE_FLOW)
	add_definitions(-DUSE_IMGUI_NODE_FLOW)
endif()

if (USE_IMGUI_NODE_EDITOR)
	add_definitions(-DUSE_IMGUI_NODE_EDITOR)
endif()

if (USE_IMGUI_MARKDOW)
	add_definitions(-DUSE_IMGUI_MARKDOW)
endif()

if (USE_IMGUI_COLOR_TEXT_EDIT)
	add_definitions(-DUSE_IMGUI_COLOR_TEXT_EDIT)
endif()
 