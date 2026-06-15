add_library(UnityInspector SHARED
    dllmain.cpp
    features/features.cpp
    features/debug_console/debug_console.cpp
    features/lua_system/lua_bindings.cpp
    features/lua_system/lua_plugin.cpp
    features/lua_system/lua_system.cpp
    features/assembly_explorer/assembly_explorer.cpp
    features/inspector/inspector_esp.cpp
    features/inspector/field_editor.cpp
    features/inspector/inspector.cpp
    features/inspector/utils.cpp
    features/memory_scanner/memory_scanner.cpp
    features/inspector/hierarchy_window.cpp
    features/inspector/inspector_window.cpp
    features/inspector/invoke_popup.cpp
    features/tests/tests.cpp
    helper/helper.cpp
    hooks/hooks.cpp
    hooks/console_hooks/console_hooks.cpp
    menu/menu.cpp
    menu/misc_tab/misc_tab.cpp
    menu/debug_tab/debug_tab.cpp
    menu/lua_tab/lua_tab.cpp
    window/window.cpp
    window/input.cpp
    window/window_finder.cpp
    window/external_overlay.cpp
    window/input_forwarder.cpp
    loader/loader.cpp
    pch/pch.cpp
)

target_include_directories(UnityInspector PUBLIC
    "${PROJECT_SOURCE_DIR}/UnityInspector/src"
    "${PROJECT_SOURCE_DIR}/UnityInspector/src/pch"
)

target_precompile_headers(UnityInspector PRIVATE pch/pch.h)

target_link_libraries(UnityInspector PRIVATE
        imgui
        kiero
        "${PROJECT_SOURCE_DIR}/UnityInspector/library/detours/detours.lib"
        "${PROJECT_SOURCE_DIR}/UnityInspector/library/luajit/lua51.lib"
)

target_compile_definitions(UnityInspector PRIVATE
    $<$<CONFIG:DEBUG>:_DEBUG>
    $<$<CONFIG:RELEASE>:NDEBUG>
    UnityInspector_EXPORTS
    _WINDOWS
    _USRDLL
)


if(MSVC)
    target_compile_options(UnityInspector PRIVATE /bigobj /EHa)
    target_compile_options(UnityInspector PRIVATE
            $<$<CONFIG:RELEASE>:/O2>
            $<$<CONFIG:RELEASE>:/GL>
            $<$<CONFIG:RELEASE>:/GS->
            $<$<CONFIG:RELEASE>:/GR->
    )
    target_link_options(UnityInspector PRIVATE 
        $<$<CONFIG:RELEASE>:/OPT:NOICF>
        $<$<CONFIG:RELEASE>:/LTCG>
        $<$<CONFIG:DEBUG>:/NODEFAULTLIB:LIBCMT>
        $<$<CONFIG:DEBUG>:/NODEFAULTLIB:LIBCMTD>
    )
endif()


add_custom_command(TARGET UnityInspector POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_SOURCE_DIR}/dist"
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:UnityInspector>" "${PROJECT_SOURCE_DIR}/dist/winhttp.dll"
        COMMAND powershell -NoProfile -Command "Copy-Item -Path '$<TARGET_PDB_FILE:UnityInspector>' -Destination '${PROJECT_SOURCE_DIR}/dist/winhttp.pdb' -Force -ErrorAction SilentlyContinue; exit 0"
        COMMAND powershell -NoProfile -Command "Copy-Item -Path '$<TARGET_FILE_DIR:UnityInspector>/*.idb' -Destination '${PROJECT_SOURCE_DIR}/dist/' -Force -ErrorAction SilentlyContinue; exit 0"
        COMMENT "Installing UnityInspector to dist/winhttp.dll..."
)