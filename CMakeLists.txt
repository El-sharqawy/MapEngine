cmake_minimum_required(VERSION 3.23)
project(MapEngine LANGUAGES C CXX)

# Set the C standard to C23 or latest
if (MSVC)
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    # This ensures C flags ONLY go to .c files
    add_compile_options($<$<COMPILE_LANGUAGE:C>:/std:clatest>)
    # This ensures CXX flags ONLY go to .cpp files
    add_compile_options($<$<COMPILE_LANGUAGE:CXX>:/std:c++20>)
    add_compile_options(/utf-8)        # Treat source as UTF-8
else()
	set(CMAKE_C_STANDARD 23)
	set(CMAKE_C_STANDARD_REQUIRED ON)
	set(CMAKE_C_EXTENSIONS ON) # Set to ON if you want 'gnu23' features
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED YES)
    add_compile_options(/utf-8)        # Treat source as UTF-8
endif()

set(CMAKE_BUILD_TYPE Debug)

# Grab all sources recursively
file(GLOB_RECURSE MapEngine_Sources 
     CONFIGURE_DEPENDS 
     "${CMAKE_CURRENT_SOURCE_DIR}/*.cpp"
)

# Grab all headers recursively
file(GLOB_RECURSE MapEngine_Headers
     CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/*.h"
)

# Then filter out the build directory
list(FILTER MapEngine_Sources EXCLUDE REGEX "${CMAKE_CURRENT_BINARY_DIR}")
list(FILTER MapEngine_Headers EXCLUDE REGEX "${CMAKE_CURRENT_BINARY_DIR}")

# Remove anything under Extern/ from the source list
list(FILTER MapEngine_Sources EXCLUDE REGEX ".*/[Ee]xtern/.*")

include_directories(BEFORE "${CMAKE_SOURCE_DIR}/Extern/include")

# 2. Tell CMake where the compiled .a or .so or .lib files are
link_directories("${CMAKE_SOURCE_DIR}/Extern/lib")
include_directories("${CMAKE_CURRENT_SOURCE_DIR}")

# 1. Add the Libraries first so other things can link to it
add_subdirectory(CoreEngine)

add_executable(MapEngine)

target_sources(MapEngine
	PRIVATE
		${MapEngine_Sources}

	PUBLIC
	FILE_SET HEADERS
	BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} # Use absolute path for safety
	FILES
		${MapEngine_Headers}	        
)

# This ensures that library knows where its own headers are during its own compilation
target_include_directories(MapEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})
# target_include_directories(MapEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/CoreEngine/include)
target_include_directories(MapEngine PRIVATE "Extern/include")

# target_precompile_headers(MapEngine PRIVATE Stdafx.h)

# Tells the compiler to optimize for the current CPU architecture (AVX2/FMA)
if(NOT MSVC)
    add_compile_options(
    -march=native
    $<$<CONFIG:Debug>:-O0 -g>
    $<$<CONFIG:Release>:-O3>
)
endif()

target_link_libraries(MapEngine PUBLIC 
    CoreEngine
)

if (WIN32)
target_link_libraries(MapEngine PRIVATE 
    # External libraries (Windows specific)
    $<$<CONFIG:Debug>:"${CMAKE_SOURCE_DIR}/Extern/lib/Windows/glfw3_d"> # .lib
    $<$<CONFIG:Release>:"${CMAKE_SOURCE_DIR}/Extern/lib/Windows/glfw3"> # .lib
    # Other standard Windows system libs
    opengl32.lib
    gdi32.lib 
    user32.lib 
    shell32.lib
)
else()
target_link_libraries(MapEngine PRIVATE 
    $<$<CONFIG:Debug>:"${CMAKE_SOURCE_DIR}/Extern/lib/Linux/libglfw_d.so">
    $<$<CONFIG:Release>:"${CMAKE_SOURCE_DIR}/Extern/lib/Linux/libglfw.so">
    GL
    dl
    m
    pthread
)
endif()
