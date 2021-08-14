cmake_minimum_required(VERSION 3.1)
include(${CMAKE_ROOT}/Modules/ExternalProject.cmake)
project(PythonEmbeddedExample)

# Check for C++11
set(CMAKE_CXX_STANDARD 11)

# CYTHON
set(CPYTHON_STDLIB_DIR ${CMAKE_SOURCE_DIR}/libs/cpython/Lib)
set(CPYTHON_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libs/cpython/Include ${CMAKE_SOURCE_DIR}/libs/cpython)
set(CPYTHON_LIBRARY_DIR ${CMAKE_SOURCE_DIR}/libs/cpython)
set(CPYTHON_LIBRARY python3.8m)
set(CPYTHON_BIN ${CMAKE_SOURCE_DIR}/libs/cpython/libpython3.8m.so)
link_directories(${CPYTHON_LIBRARY_DIR})


# PYBIND
set(PYBIND_INCLUDE_DIR ${CMAKE_SOURCE_DIR}/libs/pybind11/include)

# Build main.cpp
message(AUTHOR_WARNING "SOURCES = ${SOURCES}")
add_executable(${PROJECT_NAME} ${CMAKE_CURRENT_SOURCE_DIR}/src/main.cpp)
target_link_libraries(${PROJECT_NAME} ${CPYTHON_LIBRARY})

# Dependencies
add_dependencies(${PROJECT_NAME} PYBIND)
add_dependencies(${PROJECT_NAME} CPYTHON)
target_include_directories(${PROJECT_NAME} PRIVATE ${CPYTHON_INCLUDE_DIR})
target_include_directories(${PROJECT_NAME} PRIVATE ${PYBIND_INCLUDE_DIR})

# Copy Python DLL to the build folder if different
add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND}
  -E copy_if_different ${CPYTHON_BIN} $<TARGET_FILE_DIR:${PROJECT_NAME}>
)

# Copy our python sources to the build folder
add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND}
  -E copy_directory ${CMAKE_CURRENT_SOURCE_DIR}/src/app $<TARGET_FILE_DIR:${PROJECT_NAME}>/app
)

# Copy the Python stdlib into the build folder (needed by the embedded python)
add_custom_command(
  TARGET ${PROJECT_NAME}
  POST_BUILD
  COMMAND ${CMAKE_COMMAND}
  -E copy_directory ${CPYTHON_STDLIB_DIR} $<TARGET_FILE_DIR:${PROJECT_NAME}>/lib
)
