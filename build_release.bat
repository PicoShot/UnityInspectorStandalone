@echo off
setlocal

set "VSWHERE=%ProgramFiles(x86)%\Microsoft Visual Studio\Installer\vswhere.exe"

if not exist "%VSWHERE%" (
    echo ERROR: vswhere.exe not found. Is Visual Studio installed?
    exit /b 1
)

for /f "usebackq tokens=*" %%i in (
    `"%VSWHERE%" -latest -products * -requires Microsoft.Component.MSBuild -property installationPath`
) do (
    set "VS_PATH=%%i"
)

if not defined VS_PATH (
    echo ERROR: No suitable Visual Studio installation found.
    exit /b 1
)

echo Found Visual Studio: %VS_PATH%

set "VSDEVCMD=%VS_PATH%\Common7\Tools\VsDevCmd.bat"

if not exist "%VSDEVCMD%" (
    echo ERROR: VsDevCmd.bat not found at: %VSDEVCMD%
    exit /b 1
)

call "%VSDEVCMD%"

msbuild UnityInspector.sln /p:Configuration=Release /p:Platform=x64 /m

if %ERRORLEVEL% neq 0 (
    echo Build failed!
    exit /b %ERRORLEVEL%
)

echo Build succeeded!
endlocal