cmake_minimum_required(VERSION 3.10)

project(SurveyPropagation
        DESCRIPTION "C++ implementation of the Survey Propagation algorithm")
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)
    # Set variables.
    set(CMAKE_CXX_STANDARD 17)
    set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_LIBRARY_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/lib)
    set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)
    find_package(Doxygen)
    if(Doxygen_FOUND)
        message(STATUS "Doxygen was found, building docs")
        CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in ${CMAKE_CURRENT_BINARY_DIR}/docs/Doxyfile @ONLY)
        add_custom_target(doc ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/docs/Doxyfile
                          WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs
                          COMMENT "Generating doxygen documentation"
                          VERBATIM)
    else()
        message(STATUS "Doxygen not found, not building docs")
    endif()
endif()

# The source code is in src directory.
add_subdirectory(src)
# Libraries to lib directory and executables to bin directory.
set_target_properties(SP
        PROPERTIES
        ARCHIVE_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        LIBRARY_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/lib"
        RUNTIME_OUTPUT_DIRECTORY "${CMAKE_BINARY_DIR}/bin"
        )
target_compile_definitions(SP PRIVATE CNF_PATH="${CMAKE_SOURCE_DIR}/cnf")