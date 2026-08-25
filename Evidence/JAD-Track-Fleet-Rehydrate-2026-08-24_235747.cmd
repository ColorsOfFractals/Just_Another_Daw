@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

echo ============================================================
echo JAD EXP-040R - PUT THE PULSE BACK
echo ============================================================

echo.
echo [1/2] CHECKING REHYDRATED SESSION
cmake -S "C:\Users\benba\Tinkering\Just_Another_Daw" -B "C:\Users\benba\Tinkering\Just_Another_Daw\Build\JUCE-Foundation"
if errorlevel 1 exit /b 10

echo.
echo [2/2] REFORGING THE TRACK FLEET
cmake --build "C:\Users\benba\Tinkering\Just_Another_Daw\Build\JUCE-Foundation" --config Debug
if errorlevel 1 exit /b 20

exit /b 0
