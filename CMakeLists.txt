cmake_minimum_required(VERSION 3.10)
project(SurveyPropagation
        DESCRIPTION "C++ implementation of the Survey Propagation algorithm")
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

    # Optionally set things like CMAKE_CXX_STANDARD, CMAKE_POSITION_INDEPENDENT_CODE here
    set(CMAKE_CXX_STANDARD 17)

    # Let's nicely support folders in IDE's
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    # Docs only available if this is the main app
    find_package(Doxygen)
    if(Doxygen_FOUND)
        message(STATUS "Doxygen was found, building docs")
        CONFIGURE_FILE(${CMAKE_CURRENT_SOURCE_DIR}/docs/Doxyfile.in
                ${CMAKE_CURRENT_BINARY_DIR}/docs/Doxyfile @ONLY)
        add_custom_target(doc ALL ${DOXYGEN_EXECUTABLE} ${CMAKE_CURRENT_BINARY_DIR}/docs/Doxyfile
                WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}/docs
                COMMENT "Generating doxygen documentation"
                VERBATIM)

    else()
        message(STATUS "Doxygen not found, not building docs")
    endif()
endif()

# The source code is here
add_subdirectory(src)
