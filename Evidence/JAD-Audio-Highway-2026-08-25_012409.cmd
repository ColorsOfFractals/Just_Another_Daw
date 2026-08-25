@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64
if errorlevel 1 exit /b %errorlevel%

cd /d "C:\Users\benba\Tinkering\Just_Another_Daw"

echo ============================================================
echo JAD EXP-043 - AUDIO HIGHWAY
echo ============================================================

echo.
echo [1/2] DRAWING THE ROAD
cmake -S "C:\Users\benba\Tinkering\Just_Another_Daw" -B "C:\Users\benba\Tinkering\Just_Another_Daw\Build\JUCE-Foundation"
if errorlevel 1 exit /b %errorlevel%

echo.
echo [2/2] DRIVING THE SHONK
cmake --build "C:\Users\benba\Tinkering\Just_Another_Daw\Build\JUCE-Foundation" --config Debug
exit /b %errorlevel%
