//
// Created by antoniomanuelfr on 3/25/20.
//

#include "FactorGraph.h"

FactorGraph::FactorGraph(const FactorGraph &fc) {
    this->PositiveVariables.clear();
    this->NegativeVariables.clear();
    this->PositiveClauses.clear();
    this->NegativeClauses.clear();
    this->EdgeWeights.clear();

    this->PositiveClauses = fc.PositiveClauses;
    this->NegativeClauses = fc.NegativeClauses;
    this->PositiveVariables = fc.PositiveVariables;
    this->NegativeVariables = fc.NegativeVariables;
    this->EdgeWeights = fc.EdgeWeights;

    this->NumberClauses = fc.NumberClauses;
    this->NumberVariables = fc.NumberVariables;
}

[[maybe_unused]] FactorGraph::FactorGraph(const std::string &path, int seed) {
    int n_clauses = 0, n_variables = 0;
    ReadDIMACS(path, n_clauses, n_variables);
    this->NumberClauses = n_clauses;
    this->NumberVariables = n_variables;
    bool generate_rand = seed != -1;
    LoadEdgeWeights(generate_rand, seed);
}

void FactorGraph::SetEdgeW(unsigned int search_clause, unsigned int variable, double value) {
    if (search_clause < this->NumberClauses && variable < this->NumberVariables)
        this->EdgeWeights[search_clause][variable] = std::pair<int, double>(variable, value);
}

double FactorGraph::GetEdgeW(unsigned int search_clause, unsigned int variable) const {
    if (search_clause < this->NumberClauses && variable < this->NumberVariables)
        return this->EdgeWeights[search_clause][variable].second;
    else
        return 0.0;
}

void FactorGraph::ReadDIMACS(const std::string &path, int &n_clauses, int &n_variables) {
    std::ifstream input_file(path);
    std::string line;
    if (input_file.is_open()) {
        // Clear vectors.
        this->PositiveClauses.clear();
        this->NegativeClauses.clear();
        this->EdgeWeights.clear();
        this->PositiveVariables.clear();
        this->NegativeVariables.clear();

        // Skip the comments.
        while (getline(input_file, line) && line[0] == 'c');
        // Split the string
        std::vector<std::string> split = SplitString(line);

        // Check if the first line after the comments is the DIMACS header.
        if (!split.empty() && split[0] == "p" && split[1] == "cnf") {
            n_variables = std::stoi(split[2]);
            n_clauses = std::stoi(split[3]);
            int counter = 0, actual_value, i;
            // This are auxiliary vectors that stores the adjacency list of the actual clause
            uvector positive_adjacency_vector, negative_adjacency_vector;
            uvector *selected_adjacency_vector; // This pointer helps to not repeat code.
            // This are auxiliary vectors that stores the clauses where appear the actual variable
            umatrix negative_variables(n_variables);
            umatrix positive_variables(n_variables);
            umatrix *selected_variables_vector; // The same as the above pointer
            // We start reading the clauses section of the file
            while (getline(input_file, line) && counter < n_clauses) {
                split = SplitString(line);
                // We process only the line that are not comments
                if (split[0] != "c") {
                    i = 0;
                    // We start processing the actual clause using the split vector
                    while (split[i] != "0" && i < n_variables) {
                        actual_value = std::stoi(split[i]);
                        // If the actual value is positive we need to storage the info in the right vector
                        selected_adjacency_vector = actual_value > 0 ? &positive_adjacency_vector :
                                                    &negative_adjacency_vector;
                        selected_variables_vector = actual_value > 0 ? &positive_variables :
                                                    &negative_variables;
                        // We save the information in the right vectors.
                        selected_adjacency_vector->push_back(abs(actual_value));
                        // The variables goes from one to number of variables. We transform them to store them in
                        // a vector.
                        (*selected_variables_vector)[abs(actual_value) - 1].push_back(counter);
                        i++;
                    }
                    //It's needed to push even when empty,
                    this->PositiveClauses.push_back(positive_adjacency_vector);
                    this->NegativeClauses.push_back(negative_adjacency_vector);

                    if (!positive_adjacency_vector.empty()) {
                        positive_adjacency_vector.clear();
                    }
                    if (!negative_adjacency_vector.empty()) {
                        negative_adjacency_vector.clear();
                    }                                               
                    counter++;
                }
            }
            this->PositiveVariables = positive_variables;
            this->NegativeVariables = negative_variables;
        }else{
            std::cerr << "Enter a valid DIMACS file" << std::endl;
        }
    }else{
        std::cerr << "File not found" << std::endl;
    }
    std::cout << "File: " << path << " read correctly." << std::endl;
    input_file.close();
}

int FactorGraph::Connection(unsigned int search_clause, unsigned int variable, bool &positive) const {
    auto it = std::find(this->PositiveClauses[search_clause].cbegin(),
                        this->PositiveClauses[search_clause].cend(), variable);
    auto pos = -1;
    // If the variable was founded, we have to find the index
    if (this->PositiveClauses[search_clause].cend() != it) {
        // The complexity of this function is constant if the iterators are random access iterators
        pos = std::distance(this->PositiveClauses[search_clause].cbegin(), it);
        positive = true;
    } else {
        it = std::find(this->NegativeClauses[search_clause].cbegin(),
                       this->NegativeClauses[search_clause].cend(), variable);
        if (this->NegativeClauses[search_clause].cend() != it) {
            // The complexity of this function is constant if the iterators are random access iterators
            pos = std::distance(this->NegativeClauses[search_clause].cbegin(), it);
            positive = false;
        }
    }
    return pos;
}

void FactorGraph::LoadEdgeWeights(bool rand, unsigned long seed) {
    if (!this->EdgeWeights.empty())
        this->EdgeWeights.clear();

    std::default_random_engine generator(seed); // Random engine generator.
    std::uniform_real_distribution<double> distribution(0,1); //Distribution for the random generator.
    // Reserve memory
    this->EdgeWeights.resize(this->NumberClauses);
    int pos;
    bool positive;
    for(int i = 0; i < this->NumberClauses; i++) {
        for (int j = 1; j <= this->NumberVariables; j++) {
            // j will be the index of the variable j
            pos = this->Connection(i, j, positive);
            // If there is a positive connection between i clause and j variable
            if (pos != -1 && positive) {
                double value = rand ? (distribution(generator)) : 1.0;
                this->EdgeWeights[i].push_back(std::pair<int, double>(j, value));
            } else {
                // If there is a negative connection between i clause and j variable
                if (pos != -1 && !positive) {
                    double value = rand ? distribution(generator) : 1.0;
                    this->EdgeWeights[i].push_back(std::pair<int, double>(-j, value));
                }
            }
        }
    }
}

void FactorGraph::ApplyNewClauses(const std::vector<std::vector<int>> &deleted, const std::vector<bool> &satisfied) {
    unsigned int del = 0;
    uvector *selected_clauses, *selected_variables;
    clause cl;
    // First, remove the variables that are going to be deleted from the clauses.
    for (int clause = 0; clause < deleted.size(); clause++) {
        for (auto del_v : deleted[clause]) {
            // Selection of the positive vectors.
            selected_clauses = del_v > 0 ? &this->PositiveClauses[clause] : &this->NegativeClauses[clause];
            // Find the variable to delete in the correct clause.
            selected_clauses->erase(std::find(selected_clauses->begin(), selected_clauses->end(), del));
            // We delete the weight of the edge.
            this->EdgeWeights[clause].erase(std::find_if(this->EdgeWeights[clause].begin(), this->EdgeWeights[clause].end(),
                                       [del_v](const std::pair<int, double>& edge) {return edge.first == del_v;}));
        }
    }
    // Remove the satisfied clauses.
    for (int i = 0; i < satisfied.size(); i++) {
        if (satisfied[i]) {
            cl = this->Clause(i - del);
            this->PositiveClauses.erase(this->PositiveClauses.begin() + (i - del));
            this->NegativeClauses.erase(this->NegativeClauses.begin() + (i - del));
            del++;
        }
    }
    for (auto &v : this->PositiveVariables) {
        v.clear();
    }
    for (auto &v : this->NegativeVariables) {
        v.clear();
    }

    for (int clause = 0; clause < this->PositiveClauses.size(); clause++) {
        cl = this->Clause(clause);
        for (auto var : cl) {
            selected_variables = var > 0 ? &this->PositiveVariables[var - 1] : &this->NegativeVariables[abs(var) - 1];
            selected_variables->push_back(clause);
        }
    }
    this->NumberClauses = this->PositiveClauses.size();
}

FactorGraph FactorGraph::PartialAssignment(const std::vector<int> &assignment) {
    if (assignment.size() != this->NumberVariables)
        return FactorGraph();

    bool type;
    int index;
    std::vector<bool> satisfied_clauses(this->NumberClauses, false);
    // This matrix
    std::vector<std::vector<int>> deleted_variables_from_clauses;
    FactorGraph res (*this);
    deleted_variables_from_clauses.resize(this->NumberClauses);
    for (int i = 0; i < assignment.size(); i++) {
        // If the variable i changes.
        if (assignment[i] != 0) {
            for (int search_clause = 0; search_clause < this->NumberClauses; search_clause++) {
                index = this->Connection(search_clause, i+1, type);
                // If the variable i is in search_clause.
                if (index != -1) {
                    // If the variable appears as negative and we assign it to true.
                    if (!type && assignment[i] == 1) {
                        deleted_variables_from_clauses[search_clause].push_back(-(i + 1));
                    // If the variable appears as positive and we assign it to negative.
                    } else if (type && assignment[i] == -1) {
                        deleted_variables_from_clauses[search_clause].push_back(i + 1);
                    }
                    // If the variable appears as positive and we assign it to positive or
                    // If the variable appears as negative and we assign it to negative,
                    // the clause will be satisfied and deleted.
                    else {
                        satisfied_clauses[search_clause] = true;
                    }
                }
            }
        }
    }
    this->ApplyNewClauses(deleted_variables_from_clauses, satisfied_clauses);
    return res;
}

clause FactorGraph::Clause(unsigned int search_clause, unsigned int ignore) const {
    clause ret_clause;
    if (search_clause < this->NumberClauses) {
        for (auto it : this->PositiveClauses[search_clause]) {
            if (it != ignore)
                ret_clause.push_back(it);
        }

        for (auto it : this->NegativeClauses[search_clause])
            if (it != ignore)
                ret_clause.push_back(-it);
    }
    return ret_clause;
}

uvector FactorGraph::ClausesOfVariable(unsigned int variable) const {
    uvector ret_clause;
    if (variable < this->NumberVariables) {
        ret_clause = this->PositiveVariables[variable - 1];

        for (auto it : this->NegativeVariables[variable - 1])
            ret_clause.push_back(it);
    }
    return ret_clause;
}

void FactorGraph::VariablesInClause(unsigned int clause, uvector &positives, uvector &negatives) const {
    if (!positives.empty())
        positives.clear();

    if (!negatives.empty())
        negatives.clear();

    positives.insert(positives.begin(), this->PositiveClauses[clause].cbegin(),
                     this->PositiveClauses[clause].cend());
    negatives.insert(negatives.begin(), this->NegativeClauses[clause].cbegin(),
                     this->NegativeClauses[clause].cend());
}

std::ostream &operator<<(std::ostream &out, const FactorGraph &graph) {
    out << "p cnf " << graph.NumberVariables << " " << graph.NumberClauses << std::endl;
    for (int i = 0; i < graph.NumberClauses; i++) {
        for (int it : graph.Clause(i)) {
            out << it << " ";
        }
        out << "0" << std::endl;
    }
    return out;
}

std::vector<std::string> SplitString(const std::string &str, char delim) {
    if (str.empty()){
        return std::vector<std::string>();
    }
    // Vector with the sub-strings
    std::vector<std::string> cont;
    std::size_t current, previous = 0;
    // Find the first delimiter
    current = str.find(delim);
    while (current != std::string::npos) {
        cont.push_back(str.substr(previous, current - previous));
        previous = current + 1;
        current = str.find(delim, previous);
    }
    cont.push_back(str.substr(previous, current - previous));
    return cont;
}
