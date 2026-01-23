cmake_minimum_required(VERSION 3.14)
include(FetchContent)
project(SimpleSatSolver)


option(ENABLE_COVERAGE "Enable coverage reporting" OFF)

if(ENABLE_COVERAGE)
    set(COVERAGE_FLAGS "--coverage -fprofile-arcs -ftest-coverage")
    set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${COVERAGE_FLAGS}")
    set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${COVERAGE_FLAGS}")
endif()

# GoogleTest requires at least C++14
set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

# Also requires one of: libbfd (gnu binutils), libdwarf, libdw (elfutils)
FetchContent_Declare(backward
        GIT_REPOSITORY https://github.com/bombela/backward-cpp
        GIT_TAG master)
FetchContent_MakeAvailable(backward)

# iterator library for zip and enumerate
string(COMPARE EQUAL ${CMAKE_CXX_COMPILER_ID} "Clang" IS_CLANG)
string(COMPARE LESS ${CMAKE_CXX_COMPILER_VERSION} "16" LESS_THAN_16)
# Clangs ranges implementation is broken until clang-16 => only limited support of c++20 view features
if (${IS_CLANG} AND ${LESS_THAN_16})
    add_compile_definitions(__STD_RANGES_DISABLED__)
endif ()

FetchContent_Declare(iterators
        GIT_REPOSITORY https://github.com/Timmifixedit/IteratorTools
        GIT_TAG master
)
FetchContent_MakeAvailable(iterators)
include_directories(${iterators_SOURCE_DIR})

add_compile_definitions("$<$<BOOL:${MSVC}>:__PRETTY_FUNCTION__=__FUNCSIG__>")
set(BASE_FLAGS "$<IF:$<BOOL:${MSVC}>,/W4,-Wall;-Wextra;-Wpedantic;-mtune=native;-march=native>")
set(DEBUG_FLAGS "$<IF:$<BOOL:${MSVC}>,/fsanitize=address;/Zi,-fsanitize=address;-fno-omit-frame-pointer;-g>")
set(RELEASE_FLAGS "$<IF:$<BOOL:${MSVC}>,/O2,-O3>")
add_compile_options("${BASE_FLAGS};$<$<CONFIG:Debug>:${DEBUG_FLAGS}>$<$<CONFIG:Release>:${RELEASE_FLAGS}>")
add_link_options("$<$<CONFIG:Debug>:-fsanitize=address>")

file(GLOB_RECURSE SOURCES ${CMAKE_SOURCE_DIR}/Solver/*.cpp)
file(GLOB TARGETS ${CMAKE_SOURCE_DIR}/*.cpp)

message("generating following targets")
foreach (TARGET ${TARGETS})
    get_filename_component(NAME ${TARGET} NAME_WLE)
    message(\t${TARGET}\ ->\ target:\ ${NAME})
    add_executable(${NAME} ${TARGET} ${SOURCES} "$<$<CONFIG:Debug>:${BACKWARD_ENABLE}>")
    target_link_libraries(${NAME} PUBLIC "$<$<CONFIG:Debug>:Backward::Interface>")
endforeach ()

add_subdirectory(Tests)

