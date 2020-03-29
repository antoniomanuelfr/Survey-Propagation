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
        // Clear vectors.
        this->PositiveVariables.clear();
        this->NegativeVariables.clear();
        this->EdgeWeights.clear();
        this->PositiveClauses.clear();
        this->NegativeClauses.clear();

        // Skip the comments.
        while(getline(input_file, line) && line[0] == 'c');

        std::vector<std::string> split = SplitString(line);
        if (split[0] == "p" && split[1] == "cnf") {
            n_variables = std::stoi(split[2]);
            n_clauses = std::stoi(split[3]);
            int counter = 0, actual_value = 0, i;
            std::vector<int> positive_adjacency_vector, negative_adjacency_vector;
            std::vector<int>* selected_vector;
            std::vector<std::vector<int>> positive_clauses(n_variables), negative_clauses(n_variables);
            std::vector<std::vector<int>>* selected_clauses_vector;
            
            while (getline(input_file,line) && counter < n_clauses) {
                split = SplitString(line);
                i = 0;
                while(split[i] != "0" && i < n_variables) {
                    actual_value = std::stoi(split[i]);
                    if (actual_value > 0){
                        selected_vector= &positive_adjacency_vector;
                        selected_clauses_vector = &positive_clauses;
                    } else{
                        selected_vector= &negative_adjacency_vector;
                        selected_clauses_vector = &negative_clauses;
                    }
                    selected_vector->push_back(abs(actual_value));
                    (*selected_clauses_vector)[abs(actual_value)-1].push_back(counter);
                    i++;
                }
                //It's needed to push even when empty,
                this->PositiveVariables.push_back(positive_adjacency_vector);
                this->NegativeVariables.push_back(negative_adjacency_vector);

                if (!positive_adjacency_vector.empty()) {
                    positive_adjacency_vector.clear();
                }
                if(!negative_adjacency_vector.empty()) {
                    negative_adjacency_vector.clear();
                }
                counter++;
            }
            this->PositiveClauses = positive_clauses;
            this->NegativeClauses = negative_clauses;

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

    positives.insert(positives.begin(), this->PositiveVariables[clause].cbegin(),
            this->PositiveVariables[clause].cend());
    negatives.insert(negatives.begin(), this->NegativeVariables[clause].cbegin(),
            this->NegativeVariables[clause].cend());
}

int FactorGraph::Connection(unsigned int clause, unsigned int variable, bool &positive) const {
    auto it = std::find(this->PositiveVariables[clause].cbegin(), this->PositiveVariables[clause].cend(),
            variable);
    auto pos = -1;
    if(this->PositiveVariables[clause].cend() != it) {
        // The complexity of this function is constant if the iterators are random access iterators
        pos = std::distance(this->PositiveVariables[clause].cbegin(), it);
        positive = true;
    } else{
        it = std::find(this->NegativeVariables[clause].cbegin(), this->NegativeVariables[clause].cend(),
                variable);
        if(this->NegativeVariables[clause].cend() != it) {
            // The complexity of this function is constant if the iterators are random access iterators
            pos = std::distance(this->NegativeVariables[clause].cbegin(), it);
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
