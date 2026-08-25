@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

echo ============================================================
echo JAD EXP-038 - FIRST CLOCKWORK
echo ============================================================

echo.
echo [1/2] MAPPING TIME
cmake -S "C:\Users\benba\Tinkering\Just_Another_Daw" -B "C:\Users\benba\Tinkering\Just_Another_Daw\Build\JUCE-Foundation"
if errorlevel 1 exit /b 10

echo.
echo [2/2] PUTTING TIME THROUGH THE COMPILER
cmake --build "C:\Users\benba\Tinkering\Just_Another_Daw\Build\JUCE-Foundation" --config Debug
if errorlevel 1 exit /b 20

exit /b 0
