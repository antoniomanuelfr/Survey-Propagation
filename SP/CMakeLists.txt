cmake_minimum_required(VERSION 3.15)
project(SurveyPropagation
        DESCRIPTION "C++ implementation of the Survey Propagation algorithm")
if(CMAKE_PROJECT_NAME STREQUAL PROJECT_NAME)

    # Optionally set things like CMAKE_CXX_STANDARD, CMAKE_POSITION_INDEPENDENT_CODE here
    set(CMAKE_CXX_STANDARD 14)

    # Let's nicely support folders in IDE's
    set_property(GLOBAL PROPERTY USE_FOLDERS ON)

    # Docs only available if this is the main app
    find_package(Doxygen)
    if(Doxygen_FOUND)
        add_subdirectory(docs)
        message(STATUS "Doxygen was found, building docs")

    else()
        message(STATUS "Doxygen not found, not building docs")
    endif()
endif()

# The source code is here
add_subdirectory(src)

