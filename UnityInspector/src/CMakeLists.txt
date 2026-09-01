add_library(UnityInspector SHARED
    dllmain.cpp
    hooks/hooks.cpp
    hooks/console_hooks/console_hooks.cpp
    hooks/steam_hooks/steam_hooks.cpp
    loader/loader.cpp
    pch/pch.cpp
)

target_include_directories(UnityInspector PUBLIC
    "${PROJECT_SOURCE_DIR}/UnityInspector/src"
    "${PROJECT_SOURCE_DIR}/UnityInspector/src/pch"
	"${PROJECT_SOURCE_DIR}/UnityInspector/library"
)

target_precompile_headers(UnityInspector PRIVATE pch/pch.h)

target_link_libraries(UnityInspector PRIVATE
        "${PROJECT_SOURCE_DIR}/UnityInspector/library/detours/detours.lib"
)

target_compile_definitions(UnityInspector PRIVATE
    $<$<CONFIG:DEBUG>:_DEBUG>
    $<$<CONFIG:RELEASE>:NDEBUG>
    UnityInspector_EXPORTS
    _WINDOWS
    _USRDLL
)


if(MSVC)
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
