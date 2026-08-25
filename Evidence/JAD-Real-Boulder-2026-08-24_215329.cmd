@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

echo ============================================================
echo JAD EXP-031 - REAL BOULDER PROBE
echo ============================================================
echo.
echo No source mutation.
echo No repair.
echo No build.
echo CMake configuration only.
echo.

cmake -S "C:\Users\benba\Tinkering\Just_Another_Daw" -B "C:\Users\benba\Tinkering\Just_Another_Daw\Build\JUCE-Foundation"

exit /b %errorlevel%
