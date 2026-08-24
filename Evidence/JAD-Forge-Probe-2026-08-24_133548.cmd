@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

echo.
echo ============================================================
echo JAD FORGE DIRECT PROBE
echo ============================================================

echo.
echo === CL.EXE ===
where cl
cl

echo.
echo === LINK.EXE ===
where link

echo.
echo === CMAKE.EXE ===
where cmake
cmake --version

echo.
echo === WINDOWS SDK ===
echo WindowsSdkDir=%WindowsSdkDir%
echo WindowsSDKVersion=%WindowsSDKVersion%

echo.
echo === VC TOOLS ===
echo VCToolsInstallDir=%VCToolsInstallDir%

echo.
echo === END PROBE ===
