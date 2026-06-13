add_library(imgui STATIC
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_impl_dx11.cpp
    imgui/imgui_impl_dx12.cpp
    imgui/imgui_impl_win32.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
    imgui/TextEditor.cpp
)

target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(imgui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/imgui)

if(MSVC)
    target_compile_options(imgui PRIVATE /EHsc)
endif()

set(KIERO_BUILD_D3D9 OFF CACHE BOOL "" FORCE)
set(KIERO_BUILD_D3D10 OFF CACHE BOOL "" FORCE)
set(KIERO_BUILD_VULKAN OFF CACHE BOOL "" FORCE)
set(KIERO_BUILD_OPENGL OFF CACHE BOOL "" FORCE)
add_subdirectory(kiero2)


