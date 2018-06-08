cmake_minimum_required(VERSION 2.6)
project (PSML)
set(SOURCES
    src/main.cpp
    src/PipeletsTransform.cpp
    src/Pipelets.cpp    
)

add_executable(${PROJECT_NAME} ${SOURCES})
