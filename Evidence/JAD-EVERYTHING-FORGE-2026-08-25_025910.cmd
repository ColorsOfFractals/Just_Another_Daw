@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\Common7\Tools\VsDevCmd.bat" -arch=x64 -host_arch=x64
if errorlevel 1 exit /b %%errorlevel%%

cd /d "C:\Users\benba\Tinkering\Just_Another_Daw"

echo ============================================================
echo JAD EVERYTHING FORGE
echo ============================================================
echo.
echo === CMAKE VERSION ===
cmake --version
echo.
echo === MSBUILD LOCATION ===
where msbuild
echo.
echo === CXX LOCATION ===
where cl
echo.
echo === FORCE REBUILD JAD ===
echo.

cmake --build "C:\Users\benba\Tinkering\Just_Another_Daw\Build\JUCE-Foundation" --config Debug --target JAD -- /t:Rebuild /m

exit /b %%errorlevel%%
