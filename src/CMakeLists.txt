# Add factor graph library and specify the inc dir
add_library(factor_graph FactorGraph.cpp)
target_include_directories(factor_graph PRIVATE ${CMAKE_SOURCE_DIR}/inc)
target_compile_definitions(factor_graph PUBLIC SEED=789879)
# Add survey propagation library and specify the inc dir
add_library(survey_propagation SurveyPropagation.cpp)
target_include_directories(survey_propagation PRIVATE ${CMAKE_SOURCE_DIR}/inc)

# Link survey propagation with factor graph
target_link_libraries(survey_propagation PRIVATE factor_graph)

add_executable(SP main.cpp)
target_include_directories(SP PRIVATE ${CMAKE_SOURCE_DIR}/inc)
target_link_libraries(SP PRIVATE factor_graph survey_propagation)
find_package(OpenMP)
if(OpenMP_CXX_FOUND)
    message(STATUS "OPENMP founded")
    target_link_libraries(SP PRIVATE OpenMP::OpenMP_CXX)
endif()
