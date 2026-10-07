@echo off
rem Сборка и запуск тестов. Нужны CMake и компилятор (Visual Studio Build Tools или MinGW).
cmake -S . -B build
if errorlevel 1 exit /b 1
cmake --build build --config Release
if errorlevel 1 exit /b 1
ctest --test-dir build -C Release --output-on-failure
