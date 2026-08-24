@echo off
call "C:\Program Files (x86)\Microsoft Visual Studio\18\BuildTools\VC\Auxiliary\Build\vcvars64.bat"

echo ============================================================
echo JAD BEDROCK BUILD
echo ============================================================

echo.
echo === SOURCE ===
echo C:\Users\benba\Tinkering\Just_Another_Daw\Bedrock\main.cpp

echo.
echo === COMPILE / LINK ===
cl /nologo /EHsc "C:\Users\benba\Tinkering\Just_Another_Daw\Bedrock\main.cpp" /Fo"C:\Users\benba\Tinkering\Just_Another_Daw\Build\Bedrock\main.obj" /Fe"C:\Users\benba\Tinkering\Just_Another_Daw\Build\Bedrock\Bedrock.exe"

echo.
echo === RESULT ===
if exist "C:\Users\benba\Tinkering\Just_Another_Daw\Build\Bedrock\Bedrock.exe" (
    echo BEDROCK_EXE_CREATED=TRUE
) else (
    echo BEDROCK_EXE_CREATED=FALSE
)

echo.
echo === RUNTIME ===
if exist "C:\Users\benba\Tinkering\Just_Another_Daw\Build\Bedrock\Bedrock.exe" (
    "C:\Users\benba\Tinkering\Just_Another_Daw\Build\Bedrock\Bedrock.exe"
)

echo.
echo === END ===
