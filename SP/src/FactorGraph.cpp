//
// Created by antoniomanuelfr on 3/25/20.
//

#include "../inc/FactorGraph.h"

FactorGraph::FactorGraph(const std::string& path, int seed) {
    int n_clauses, n_variables;
    ReadDIMACS(path, n_clauses, n_variables);
    this->NumberClauses = n_clauses;
    this->NumberVariables = n_variables;
    bool generate_rand = seed != -1;
    LoadEdgeWeights(generate_rand, seed);
}

void FactorGraph::ReadDIMACS(const std::string &path, int& n_clauses, int& n_variables) {
    std::ifstream input_file (path);
    std::string line;
    if (input_file.is_open()){
        // Check if the vectors are not empty
        if (!this->PositiveNodes.empty() || !this->NegativeNodes.empty() || !this->EdgeWeights.empty()) {
            this->PositiveNodes.clear();
            this->NegativeNodes.clear();
            this->EdgeWeights.clear();
        }
        // Skip the comments.
        while(getline(input_file, line) && line[0] == 'c');
        std::vector<std::string> split = SplitString(line);
        if (split[0] == "p" && split[1] == "cnf") {
            n_variables = std::stoi(split[2]);
            n_clauses = std::stoi(split[3]);
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
                this->PositiveNodes.push_back(positive_adjacency_list);
                this->NegativeNodes.push_back(negative_adjacency_list);

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

void FactorGraph::VariablesInClause(int clause, std::vector<int>& positives, std::vector<int>& negatives) const {
    if(!positives.empty())
        positives.clear();

    if(!negatives.empty())
        negatives.clear();

    positives.insert(positives.begin(),this->PositiveNodes[clause].cbegin(), this->PositiveNodes[clause].cend());
    negatives.insert(negatives.begin(),this->NegativeNodes[clause].cbegin(), this->NegativeNodes[clause].cend());
}

int FactorGraph::Connection(unsigned int clause, unsigned int variable, bool &positive) const {
    auto it = std::find(this->PositiveNodes[clause].cbegin(), this->PositiveNodes[clause].cend(), variable);
    auto pos = -1;
    if(this->PositiveNodes[clause].cend() != it) {
        // The complexity of this function is constant if the iterators are random access iterators
        pos = std::distance(this->PositiveNodes[clause].cbegin(), it);
        positive = true;
    } else{
        it = std::find(this->NegativeNodes[clause].cbegin(), this->NegativeNodes[clause].cend(), variable);
        if(this->NegativeNodes[clause].cend() != it) {
            // The complexity of this function is constant if the iterators are random access iterators
            pos = std::distance(this->NegativeNodes[clause].cbegin(), it);
            positive = false;
        }
    }
    return pos;
}

void FactorGraph::LoadEdgeWeights(bool rand, unsigned long seed) {
    if (!this->EdgeWeights.empty()) {
        this->EdgeWeights.clear();
    }

    std::default_random_engine generator(seed);
    std::uniform_real_distribution<double> distribution(0,1);

    this->EdgeWeights.resize(this->NumberClauses);
    int pos;
    bool positive;
    for(int i = 0; i < this->NumberClauses; i++) {
        for(int j = 0; j < this->NumberVariables; j++) {
            // j will be the index of the variable j+1
            pos = this->Connection(i, j+1, positive);
            if(pos !=-1 && positive) {
                double value = rand ? (distribution(generator)) : 1.0;
                this->EdgeWeights[i].push_back(value);
            } else {
                if (pos != -1 && !positive) {
                    double value = rand ? distribution(generator) : 1.0;
                    this->EdgeWeights[i].push_back(value);
                }
            }
        }
    }
}

std::vector<std::string> SplitString(const std::string& str, char delim) {
    std::vector<std::string> cont;
    std::size_t current, previous = 0;
    current = str.find(delim);
    while (current != std::string::npos) {
        cont.push_back(str.substr(previous, current - previous));
        previous = current + 1;
        current = str.find(delim, previous);
    }
    cont.push_back(str.substr(previous, current - previous));
    return cont;
}
