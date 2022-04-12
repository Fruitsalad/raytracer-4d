# The structure of this project is inspired by github.com/bsamseth/cpp-project
cmake_minimum_required(VERSION 3.16)

project(raytracer_4d)

# Put together a few convenient variables
set(HERE ${CMAKE_CURRENT_SOURCE_DIR}) # The absolute path of this folder.
set(BIN ${CMAKE_CURRENT_BINARY_DIR}) # The absolute path of the build folder.

if (${CMAKE_SYSTEM_NAME} MATCHES "Emscripten")
    set(USING_EMSCRIPTEN true)
else()
    set(USING_EMSCRIPTEN false)
endif()

# CMake options
option(ENABLE_DOCTESTS "Include tests in the library. Setting this to OFF will remove all doctest related code. Tests in tests/*.cpp will still be enabled." ON)

# Basic configuration
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${HERE}/cmake/")
include(LTO)
find_lto(CXX)

# Dependencies
if (USING_EMSCRIPTEN)
    message("Skipping package checks because we're compiling with Emscripten.")
else()
    find_package(doctest)
    find_package(SDL2 REQUIRED)
    find_package(SDL2_ttf REQUIRED)
    find_package(Threads)
endif()

set(ENABLE_THREADS ${Threads_FOUND})

if (NOT doctest_FOUND AND ENABLE_DOCTESTS)
    message("Doctest was disabled because it was not found.")
    set(ENABLE_DOCTESTS false)
endif()


# Targets

# This target copies all the files in ./data to the build output directory
# It should run automatically when you compile the program.
add_custom_target(
        copy_data_files
        COMMAND python3 "${HERE}/copy_data_files.py" "${HERE}/data" "${BIN}"
)

# Glob all the .cpp files together into ${cpp_files}.
# CONFIGURE_DEPENDS makes globbing relatively safe, and it's a much more
# pleasant experience than manually listing the source files.
file(GLOB_RECURSE core_files CONFIGURE_DEPENDS "src/*.cpp")
file(GLOB_RECURSE test_files CONFIGURE_DEPENDS "test/*.cpp")


# The "core" target compiles all the files that are shared between different
# builds of the program.
add_library(core OBJECT ${core_files})
add_dependencies(core copy_data_files)
target_include_directories(
        core PUBLIC
        ${HERE}/src
        ${SDL2_INCLUDE_DIRS}
        ${SDL2_TTF_INCLUDE_DIRS}
)
target_link_libraries(
        core PUBLIC
        ${SDL2_LIBRARIES}
        ${SDL2_TTF_LIBRARIES}
)


if (ENABLE_THREADS)
    target_link_libraries(core PUBLIC ${CMAKE_THREAD_LIBS_INIT})
    target_compile_definitions(core PUBLIC ENABLE_THREADS)
endif()

if (ENABLE_DOCTESTS)
    # Link DocTest and add the preprocessor definition "ENABLE_DOCTEST".
    target_link_libraries(core PUBLIC doctest::doctest)
    target_compile_definitions(core PUBLIC ENABLE_DOCTEST)

    # The test build of the program.
    add_executable(test ${test_files})
    target_link_libraries(test PRIVATE core)
endif()

# Define either FRUIT_DEBUG or FRUIT_RELEASE depending on the type of build.
target_compile_definitions(core PUBLIC $<IF:$<CONFIG:Debug>, FRUIT_DEBUG=1, FRUIT_RELEASE=1>)


# The normal build of the program.
# Note: The WIN32 option is necessary to keep a console window from popping up
#       when running the program on Windows.
add_executable(raytracer_4d WIN32 main.cpp)
target_link_libraries(raytracer_4d PRIVATE core)
target_enable_lto(raytracer_4d optimized)

if (USING_EMSCRIPTEN)
    message("Adding Emscripten link flags")
    set_target_properties(
            raytracer_4d PROPERTIES
            LINK_FLAGS "-s USE_SDL=2 -s USE_SDL_TTF=2 -s USE_FREETYPE=1 --preload-file roboto.ttf"
    )
    message("Copying emscripten_canvas.html")
    file(COPY emscripten_canvas.html DESTINATION .)
    file(RENAME ${BIN}/emscripten_canvas.html ${BIN}/index.html)
endif()
