cmake_minimum_required(VERSION 2.6)
project (PSML)
set(SOURCES
    src/main.cpp
    src/PipeletsTransform.cpp
)

add_executable(${PROJECT_NAME} ${SOURCES})
