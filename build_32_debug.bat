@ECHO OFF
SETLOCAL

SET PATH=C:\Programs\mingw\mingw32\bin;%PATH%
SET PROJECT_PATH=%~dp0

SET BUILD_PATH=%~dp0Build\Win32_Debug
rd "%BUILD_PATH%" /s /q
MD "%BUILD_PATH%"
CD "%BUILD_PATH%"

cmake "-GMinGW Makefiles" "-DCMAKE_BUILD_TYPE=Debug" %PROJECT_PATH%
cmake --build . --target all