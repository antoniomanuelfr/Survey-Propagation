# Make an automatic library - will be static or dynamic based on user setting
add_library(factor_graph FactorGraph.cpp ${HEADER_LIST})
# We need this directory, and users of our library will need it too
target_include_directories(factor_graph PUBLIC ../include)
add_executable(SP main.cpp)
target_link_libraries(SP PRIVATE factor_graph)