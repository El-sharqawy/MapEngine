cmake_minimum_required(VERSION 3.23)
project(CoreEngine LANGUAGES CXX)

# Set the C++ standard to C20 or latest
if (MSVC)
    # We use 'target' level settings instead of global 'add_compile_options'
    # to prevent flag clashing.
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
else()
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED YES)
endif()

# Grab all sources recursively
file(GLOB_RECURSE CoreEngine_Sources 
     CONFIGURE_DEPENDS 
     "${CMAKE_CURRENT_SOURCE_DIR}/src/*.cpp"
)

# Grab all headers recursively
file(GLOB_RECURSE CoreEngine_Headers
     CONFIGURE_DEPENDS
     "${CMAKE_CURRENT_SOURCE_DIR}/include/*.h"
)

# Then filter out the build directory
list(FILTER CoreEngine_Sources EXCLUDE REGEX "${CMAKE_CURRENT_BINARY_DIR}")
list(FILTER CoreEngine_Headers EXCLUDE REGEX "${CMAKE_CURRENT_BINARY_DIR}")

add_library(CoreEngine STATIC)

target_sources(CoreEngine
	PRIVATE
		${CoreEngine_Sources}

	PUBLIC
	FILE_SET HEADERS
	BASE_DIRS ${CMAKE_CURRENT_SOURCE_DIR} # Use absolute path for safety
	FILES
		${CoreEngine_Headers}	        
)

# This ensures that library knows where its own headers are during its own compilation
target_include_directories(CoreEngine PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include) # exposes CoreEngine/include/
target_include_directories(CoreEngine PRIVATE "Extern/include")

target_link_libraries(CoreEngine
)

# target_precompile_headers(CoreEngine PRIVATE Stdafx.h)

# Tells the compiler to optimize for the current CPU architecture (AVX2/FMA)
if(NOT MSVC)
    add_compile_options(
    -march=native
    $<$<CONFIG:Debug>:-O0 -g>
    $<$<CONFIG:Release>:-O3>
)
endif()
