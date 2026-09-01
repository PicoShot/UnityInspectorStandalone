set(IMGUI_SOURCES
    imgui/imgui.cpp
    imgui/imgui_demo.cpp
    imgui/imgui_draw.cpp
    imgui/imgui_impl_dx11.cpp
    imgui/imgui_impl_dx12.cpp
    imgui/imgui_impl_win32.cpp
    imgui/imgui_tables.cpp
    imgui/imgui_widgets.cpp
)

if(LUA_PLUGINS)
    list(APPEND IMGUI_SOURCES imgui/TextEditor.cpp)
endif()

add_library(imgui STATIC ${IMGUI_SOURCES})

target_include_directories(imgui PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
target_include_directories(imgui PRIVATE ${CMAKE_CURRENT_SOURCE_DIR}/imgui)

if(MSVC)
    target_compile_options(imgui PRIVATE /EHsc)
endif()

add_subdirectory(kiero2)
