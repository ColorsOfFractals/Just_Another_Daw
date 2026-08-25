@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

echo ============================================================
echo JAD EXP-039 - FIRST TRACK
echo ============================================================

echo.
echo [1/2] REGISTERING MUSICAL TERRITORY
cmake -S "C:\Users\benba\Tinkering\Just_Another_Daw" -B "C:\Users\benba\Tinkering\Just_Another_Daw\Build\JUCE-Foundation"
if errorlevel 1 exit /b 10

echo.
echo [2/2] COMPILING ONE EXTREMELY LONELY TRACK
cmake --build "C:\Users\benba\Tinkering\Just_Another_Daw\Build\JUCE-Foundation" --config Debug
if errorlevel 1 exit /b 20

exit /b 0
