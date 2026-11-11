cmake_minimum_required(VERSION 3.16)
project(lab3 LANGUAGES CXX)

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

# MinGW: стандартная библиотека линкуется статически, чтобы .exe запускался без DLL компилятора.
if(MINGW)
    add_link_options(-static)
endif()

function(enable_warnings target)
    if(MSVC)
        target_compile_options(${target} PRIVATE /W4)
    else()
        target_compile_options(${target} PRIVATE -Wall -Wextra -Wpedantic)
    endif()
endfunction()

# Библиотека с классами предметной области. Она ничего не знает о консольном меню.
file(GLOB_RECURSE CORE_SOURCES CONFIGURE_DEPENDS src/*.cpp)
add_library(lab3_core STATIC ${CORE_SOURCES})
target_include_directories(lab3_core PUBLIC include)
enable_warnings(lab3_core)

if(ENABLE_COVERAGE AND NOT MSVC)
    target_compile_options(lab3_core PUBLIC --coverage -O0 -g)
    target_link_options(lab3_core PUBLIC --coverage)
endif()

# Консольная программа «Банкомат».
add_executable(atm_app app/main.cpp app/ConsoleUtils.cpp app/DemoAtm.cpp)
target_link_libraries(atm_app PRIVATE lab3_core)
enable_warnings(atm_app)

# Модульные тесты (doctest лежит в tests/doctest.h, скачивать ничего не нужно).
enable_testing()
file(GLOB TEST_SOURCES CONFIGURE_DEPENDS tests/*.cpp)
add_executable(lab3_tests ${TEST_SOURCES})
target_link_libraries(lab3_tests PRIVATE lab3_core)
add_test(NAME lab3_tests COMMAND lab3_tests)
