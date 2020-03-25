//
// Created by antoniomanuelfr on 3/25/20.
//

#include "../inc/FactorGraph.h"

const std::vector<std::vector<int>> &FactorGraph::getNegativeNodes() const {
    return NegativeNodes;
}

const std::vector<std::vector<int>> &FactorGraph::getPositiveNodes() const {
    return PositiveNodes;
}

const std::vector<std::vector<float>> &FactorGraph::getEdgePis() const {
    return EdgePis;
}

FactorGraph::FactorGraph(const std::string& path) {
    std::ifstream input_file (path);
    std::string line;
    if (input_file.is_open()){
        // Skip the comments.
        while(getline(input_file, line) && line[0] == 'c');
            std::cout << line[6] << std::endl;
        if (line[0] == 'p'){
        }else{
            std::cout << "Enter a valid .cnf file." << std::endl;
        }
    }else{
        std::cout << "File not found" << std::endl;
    }
}

