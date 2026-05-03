cmake_minimum_required(VERSION 3.10)
project(ElectronicLibrary LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 11)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Подключаем заголовочные файлы
include_directories(include)

# Исходные файлы
set(SOURCES
    src/main.cpp
    src/Book.cpp
    src/Reader.cpp
    src/Library.cpp
)

# Создаём исполняемый файл
add_executable(library ${SOURCES})
