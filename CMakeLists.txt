cmake_minimum_required(VERSION 3.13)
project(WEBXREXAMPLE)

set(GLM_INCLUDE_DIR "../glm")

add_subdirectory(emscripten-webxr)

include_directories( ${GLM_INCLUDE_DIR})
include_directories( ${SDL2_INCLUDE_DIRS} )

set(WEB_DIR, web)
set(EXE, index.html)

# Create a debug build
set(CMAKE_CXX_FLAGS "-Wall -Wformat")
set(CMAKE_CXX_FLAGS_DEBUG_INIT "-Wall -Wformat -g -fdebug-compilation-dir='..'")
set(CMAKE_CXX_FLAGS_RELEASE_INIT "-Wall -Wformat -Os")

set(USE_FLAGS "-s USE_SDL=2 --preload-file ../shaders -s USE_WEBGL2=1 -s WASM=1 -s DISABLE_EXCEPTION_CATCHING=1 -s NO_EXIT_RUNTIME=0 --shell-file ../shell_minimal.html")
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${USE_FLAGS}")
set(CMAKE_C_FLAGS "${CMAKE_C_FLAGS} ${USE_FLAGS}")
set(CMAKE_EXE_LINKER_FLAGS "${CMAKE_EXE_LINKER_FLAGS} ${USE_FLAGS}")
set(CMAKE_EXECUTABLE_SUFFIX .html)

file(GLOB_RECURSE SOURCE_FILES *.cpp)
file(GLOB_RECURSE HEADER_FILES *.h)

include_directories(${CMAKE_SOURCE_DIR})
## Create main executable
add_executable(${PROJECT_NAME} ${SOURCE_FILES})

set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-s USE_SDL=2")
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "--preload-file shaders") 
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-s ALLOW_MEMORY_GROWTH=1")
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-s DISABLE_EXCEPTION_CATCHING=1")
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-s ASSERTIONS=1")
set_target_properties(${PROJECT_NAME} PROPERTIES LINK_FLAGS "-s LLD_REPORT_UNDEFINED")

target_link_libraries(${PROJECT_NAME} ${SDL2_LIBRARIES})
target_link_libraries(${PROJECT_NAME} webxr)
