add_library(OnlineFixUnity SHARED
    dllmain.cpp
    hooks/hooks.cpp
    hooks/console_hooks/console_hooks.cpp
    hooks/steam_hooks/facepunch/facepunch_hooks.cpp
    hooks/steam_hooks/heathen/heathen_hooks.cpp
    hooks/steam_hooks/rlabrecque/rlabrecque_hooks.cpp
    loader/loader.cpp
    pch/pch.cpp
    hooks/steam_hooks/steam_hooks.cpp
)

target_include_directories(OnlineFixUnity PUBLIC
    "${PROJECT_SOURCE_DIR}/OnlineFixUnity/src"
    "${PROJECT_SOURCE_DIR}/OnlineFixUnity/src/pch"
	"${PROJECT_SOURCE_DIR}/OnlineFixUnity/library"
)

target_precompile_headers(OnlineFixUnity PRIVATE pch/pch.h)

target_link_libraries(OnlineFixUnity PRIVATE
        "${PROJECT_SOURCE_DIR}/OnlineFixUnity/library/detours/detours.lib"
)

target_compile_definitions(OnlineFixUnity PRIVATE
    $<$<CONFIG:DEBUG>:_DEBUG>
    $<$<CONFIG:RELEASE>:NDEBUG>
    OnlineFixUnity_EXPORTS
    _WINDOWS
    _USRDLL
)


if(MSVC)
    target_compile_options(OnlineFixUnity PRIVATE
            $<$<CONFIG:RELEASE>:/O2>
            $<$<CONFIG:RELEASE>:/GL>
            $<$<CONFIG:RELEASE>:/GS->
            $<$<CONFIG:RELEASE>:/GR->
    )
    target_link_options(OnlineFixUnity PRIVATE 
        $<$<CONFIG:RELEASE>:/OPT:NOICF>
        $<$<CONFIG:RELEASE>:/LTCG>
        $<$<CONFIG:DEBUG>:/NODEFAULTLIB:LIBCMT>
        $<$<CONFIG:DEBUG>:/NODEFAULTLIB:LIBCMTD>
    )
endif()


add_custom_command(TARGET OnlineFixUnity POST_BUILD
        COMMAND ${CMAKE_COMMAND} -E make_directory "${PROJECT_SOURCE_DIR}/dist"
        COMMAND ${CMAKE_COMMAND} -E copy "$<TARGET_FILE:OnlineFixUnity>" "${PROJECT_SOURCE_DIR}/dist/winhttp.dll"
        COMMAND powershell -NoProfile -Command "Copy-Item -Path '$<TARGET_PDB_FILE:OnlineFixUnity>' -Destination '${PROJECT_SOURCE_DIR}/dist/winhttp.pdb' -Force -ErrorAction SilentlyContinue; exit 0"
        COMMAND powershell -NoProfile -Command "Copy-Item -Path '$<TARGET_FILE_DIR:OnlineFixUnity>/*.idb' -Destination '${PROJECT_SOURCE_DIR}/dist/' -Force -ErrorAction SilentlyContinue; exit 0"
        COMMENT "Installing OnlineFixUnity to dist/winhttp.dll..."
)
