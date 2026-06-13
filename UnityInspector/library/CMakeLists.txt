add_library(imgui STATIC
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_impl_dx11.cpp
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

add_library(kiero STATIC
    kiero2/kiero_intern.cpp
    kiero2/kiero_d3d11.cpp
)

target_include_directories(kiero PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/kiero2)
target_compile_definitions(kiero PUBLIC KIERO_BUILD_D3D11=1)
target_compile_features(kiero PUBLIC cxx_std_17)
if(MSVC)
    target_compile_options(kiero PRIVATE /EHsc)
endif()
