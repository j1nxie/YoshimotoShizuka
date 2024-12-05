cmake_minimum_required(VERSION 3.15)
project(YoshimotoShizuka VERSION 0.1.0 LANGUAGES CXX)

# Options
option(ENABLE_TESTING "Enable testing" ON)
option(ENABLE_WARNINGS "Enable warnings" ON)
option(ENABLE_WERROR "Treat warnings as errors" OFF)

# Compiler settings
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Dependencies
find_package(nlohmann_json REQUIRED)
find_package(SQLite3 REQUIRED)

# Compiler warnings
if(ENABLE_WARNINGS)
    if(CMAKE_CXX_COMPILER_ID MATCHES "GNU|Clang")
        add_compile_options(
            -Wall
            -Wextra
            -Wpedantic
            -Wconversion
            -Wshadow
            -Wnon-virtual-dtor
            -Wold-style-cast
            -Wcast-align
            -Wunused
            -Woverloaded-virtual
            -Wsign-conversion
            -Wnull-dereference
            -Wdouble-promotion
            -Wformat=2
        )
        if(ENABLE_WERROR)
            add_compile_options(-Werror)
        endif()
    endif()
endif()

# Source files
set(SOURCES
    src/main.cpp
    src/TodoEntry.cpp
    src/TodoServer.cpp
    src/db/Migrator.cpp
    src/http/HttpRequest.cpp
    src/http/HttpResponse.cpp
    src/http/HttpRouter.cpp
)

# Main executable
add_executable(${PROJECT_NAME} ${SOURCES})

target_include_directories(${PROJECT_NAME}
    PRIVATE
        ${PROJECT_SOURCE_DIR}/include
)

target_link_libraries(${PROJECT_NAME}
    PRIVATE
        nlohmann_json::nlohmann_json
)
target_link_libraries(${PROJECT_NAME}
    PRIVATE
        SQLite::SQLite3
)

# Testing
# if(ENABLE_TESTING)
    # enable_testing()
    # add_subdirectory(tests)
# endif()
