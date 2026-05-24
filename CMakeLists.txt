cmake_minimum_required(VERSION 3.25)
project(UnityInspector)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

add_library(UnityInspector SHARED
    "UnityInspector/src/dllmain.cpp"
    "UnityInspector/library/imgui/imgui.cpp"
    "UnityInspector/library/imgui/imgui_demo.cpp"
    "UnityInspector/library/imgui/imgui_draw.cpp"
    "UnityInspector/library/imgui/imgui_impl_dx11.cpp"
    "UnityInspector/library/imgui/imgui_impl_win32.cpp"
    "UnityInspector/library/imgui/imgui_tables.cpp"
    "UnityInspector/library/imgui/imgui_widgets.cpp"
    "UnityInspector/library/imgui/TextEditor.cpp"
    "UnityInspector/library/kiero/kiero.cpp"
    "UnityInspector/src/features/features.cpp"
    "UnityInspector/src/features/debug_console/debug_console.cpp"
    "UnityInspector/src/features/lua_system/lua_bindings.cpp"
    "UnityInspector/src/features/lua_system/lua_plugin.cpp"
    "UnityInspector/src/features/lua_system/lua_system.cpp"
    "UnityInspector/src/features/assembly_explorer/assembly_explorer.cpp"
    "UnityInspector/src/features/inspector/inspector_esp.cpp"
    "UnityInspector/src/features/inspector/field_editor.cpp"
    "UnityInspector/src/features/inspector/inspector.cpp"
    "UnityInspector/src/features/inspector/utils.cpp"
    "UnityInspector/src/features/inspector/hierarchy_window.cpp"
    "UnityInspector/src/features/inspector/inspector_window.cpp"
    "UnityInspector/src/features/inspector/invoke_popup.cpp"
    "UnityInspector/src/features/tests/tests.cpp"
    "UnityInspector/src/helper/helper.cpp"
    "UnityInspector/src/hooks/hooks.cpp"
    "UnityInspector/src/hooks/console_hooks/console_hooks.cpp"
    "UnityInspector/src/menu/menu.cpp"
    "UnityInspector/src/menu/info_tab/info_tab.cpp"
    "UnityInspector/src/menu/debug_tab/debug_tab.cpp"
    "UnityInspector/src/menu/lua_tab/lua_tab.cpp"
    "UnityInspector/src/window/window.cpp"
    "UnityInspector/src/window/input.cpp"
    "UnityInspector/src/window/window_finder.cpp"
    "UnityInspector/src/window/external_overlay.cpp"
    "UnityInspector/src/window/input_forwarder.cpp"
    "UnityInspector/src/loader/loader.cpp"
    "UnityInspector/src/pch/pch.cpp"
)

if(MSVC)
    target_compile_options(UnityInspector PRIVATE /bigobj)
endif()

target_include_directories(UnityInspector PUBLIC
    "${CMAKE_CURRENT_SOURCE_DIR}/UnityInspector/src"
    "${CMAKE_CURRENT_SOURCE_DIR}/UnityInspector/library"
    "${CMAKE_CURRENT_SOURCE_DIR}/UnityInspector/src/pch"
)

target_link_libraries(UnityInspector
    "${CMAKE_CURRENT_SOURCE_DIR}/UnityInspector/library/detours/[x64]detours.lib"
    "${CMAKE_CURRENT_SOURCE_DIR}/UnityInspector/library/luajit/lua51.lib"
)

target_compile_definitions(UnityInspector PRIVATE
    $<$<CONFIG:DEBUG>:_DEBUG>
    $<$<CONFIG:RELEASE>:NDEBUG>
    UnityInspector_EXPORTS
    _WINDOWS
    _USRDLL
)

if(NOT CMAKE_CXX_COMPILER_LAUNCHER MATCHES "sccache")
    set_target_properties(UnityInspector PROPERTIES
            PCH_HEADER "pch.h"
            PCH_SOURCE "src/pch/pch.cpp"
    )
endif()

set(EXECUTABLE_OUTPUT_PATH ${CMAKE_BINARY_DIR}/bin)
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)
# test