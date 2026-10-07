cmake_minimum_required(VERSION 3.16)
project(lab1 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_CXX_EXTENSIONS OFF)

option(ENABLE_COVERAGE "Собирать с измерением покрытия кода (gcc/clang)" OFF)

# Все исполняемые файлы кладём в build/bin, независимо от компилятора и конфигурации.
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
foreach(config DEBUG RELEASE RELWITHDEBINFO MINSIZEREL)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY_${config} ${CMAKE_BINARY_DIR}/bin)
endforeach()

# Исходные файлы в кодировке UTF-8 (русские строки).
if(MSVC)
    add_compile_options(/utf-8)
endif()

function(enable_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

# Библиотека с классами Dictionary и Fifteen — она не зависит от консольного интерфейса.
add_library(lab1_core STATIC src/Dictionary.cpp src/Fifteen.cpp)
target_include_directories(lab1_core PUBLIC include)
enable_warnings(lab1_core)

if(ENABLE_COVERAGE AND NOT MSVC)
    target_compile_options(lab1_core PUBLIC --coverage -O0 -g)
    target_link_options(lab1_core PUBLIC --coverage)
endif()

# Консольные программы с меню.
add_executable(dictionary_app app/dictionary_main.cpp app/ConsoleUtils.cpp)
target_link_libraries(dictionary_app PRIVATE lab1_core)
enable_warnings(dictionary_app)

add_executable(fifteen_app app/fifteen_main.cpp app/ConsoleUtils.cpp)
target_link_libraries(fifteen_app PRIVATE lab1_core)
enable_warnings(fifteen_app)

# Модульные тесты (doctest лежит в tests/doctest.h, скачивать ничего не нужно).
enable_testing()
add_executable(lab1_tests tests/test_main.cpp tests/test_dictionary.cpp tests/test_fifteen.cpp)
target_link_libraries(lab1_tests PRIVATE lab1_core)
add_test(NAME lab1_tests COMMAND lab1_tests)
