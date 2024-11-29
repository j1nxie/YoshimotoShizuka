default:
    @just --choose

# Build variables
build_dir := "build"
build_type := env_var_or_default("BUILD_TYPE", "Debug")
compiler := env_var_or_default("CXX", "clang++")
cmake_generator := "Ninja"

# Project directories
src_dirs := "src include tests"

# Setup build directory and generate build files
setup:
    mkdir -p {{build_dir}}
    cd {{build_dir}} && cmake -G {{cmake_generator}} \
        -DCMAKE_BUILD_TYPE={{build_type}} \
        -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
        -DCMAKE_CXX_COMPILER={{compiler}} \
        ..

# Build the project
build: setup
    cd {{build_dir}} && ninja

# Clean build directory
clean:
    rm -rf {{build_dir}}

# Rebuild everything from scratch
rebuild: clean build

# Format code using clang-format
format:
    find {{src_dirs}} -type f \( -name '*.cpp' -o -name '*.hpp' \) -exec clang-format -i {} +

# Run tests
test: build
    cd {{build_dir}} && ctest --output-on-failure

# Run the application
run: build
    ./{{build_dir}}/YoshimotoShizuka

# Generate compilation database
compile-commands: setup
    cd {{build_dir}} && ninja

# Check for compiler warnings
check:
    cd {{build_dir}} && cmake -DENABLE_WARNINGS=ON -DENABLE_WERROR=ON .. && ninja

# Create a new release build
release:
    just BUILD_TYPE=Release build

# Create a debug build
debug:
    just BUILD_TYPE=Debug build

# Print system information
info:
    @echo "Compiler: {{compiler}}"
    @echo "Build type: {{build_type}}"
    @echo "CMake generator: {{cmake_generator}}"
    @cmake --version
    @{{compiler}} --version
    @ninja --version
