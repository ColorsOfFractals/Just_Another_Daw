@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64
if errorlevel 1 exit /b %%errorlevel%%

cd /d "C:\Users\benba\Tinkering\Just_Another_Daw"

echo ============================================================
echo JAD EXP-046 - HANDS ON THE TIMELINE
echo EXP-047 - TIMELINE SEED
echo ============================================================

cmake --build "C:\Users\benba\Tinkering\Just_Another_Daw\Build\JUCE-Foundation" --config Debug

exit /b %%errorlevel%%
