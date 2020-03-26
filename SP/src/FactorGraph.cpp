//
// Created by antoniomanuelfr on 3/25/20.
//

#include "../inc/FactorGraph.h"

std::vector<std::string> SplitString(const std::string& str)
{
    std::vector<std::string> cont;
    std::istringstream aux(str);
    std::copy(std::istream_iterator<std::string>(aux),std::istream_iterator<std::string>(), std::back_inserter(cont));
    return cont;
}

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
    ReadDIMACS(path);
}

void FactorGraph::ReadDIMACS(const std::string &path) {
    std::ifstream input_file (path);
    std::string line;
    if (input_file.is_open()){
        // Check if the vectors are not empty
        if (!this->PositiveNodes.empty() || !this->NegativeNodes.empty() || !this->EdgePis.empty()) {
            PositiveNodes.clear();
            NegativeNodes.clear();
        }
        // Skip the comments.
        while(getline(input_file, line) && line[0] == 'c');
        std::vector<std::string> split = SplitString(line);
        if (split[0] == "p" && split[1] == "cnf"){
            int n_variables = std::stoi(split[2]);
            int n_clauses = std::stoi(split[3]);
            int counter = 0, actual_value = 0, i;
            std::vector<int> positive_adjacency_list, negative_adjacency_list;
            std::vector<int>* selected_list;
            while (getline(input_file,line) && counter < n_clauses) {
                split = SplitString(line);
                i = 0;
                while(split[i] != "0" && i < n_variables) {
                    actual_value = std::stoi(split[i]);
                    selected_list = actual_value > 0 ? &positive_adjacency_list : &negative_adjacency_list;
                    selected_list->push_back(abs(actual_value));
                    i++;
                }
                //It's needed to push even when empty,
                PositiveNodes.push_back(positive_adjacency_list);
                NegativeNodes.push_back(negative_adjacency_list);

                if (!positive_adjacency_list.empty()) {
                    positive_adjacency_list.clear();
                }
                if(!negative_adjacency_list.empty()) {
                    negative_adjacency_list.clear();
                }
                counter++;
            }
        }else{
            std::cerr << "Enter a valid DIMACS file" << std::endl;
        }
    }else{
        std::cerr << "File not found" << std::endl;
    }
    std::cout << "File: " << path << " readed correctly." << std::endl;
}

