//
// Created by antoniomanuelfr on 3/25/20.
//

#include "FactorGraph.h"

FactorGraph::FactorGraph(const std::vector<std::vector<unsigned int>> &positiveVariables,
                         const std::vector<std::vector<unsigned int>> &negativeVariables,
                         const std::vector<std::vector<unsigned int>> &positiveClauses,
                         const std::vector<std::vector<unsigned int>> &negativeClauses,
                         const std::vector<std::vector<double>> &edgeWeights) {
    this->PositiveVariables = positiveVariables;
    this->NegativeVariables = negativeVariables;
    this->PositiveClauses = positiveClauses;
    this->NegativeClauses = negativeClauses;
    this->NumberClauses = this->PositiveVariables.size();
    this->NumberVariables = this->PositiveClauses.size();
    this->EdgeWeights = edgeWeights;
}

FactorGraph::FactorGraph(const std::string &path, int seed) {
    int n_clauses, n_variables;
    ReadDIMACS(path, n_clauses, n_variables);
    this->NumberClauses = n_clauses;
    this->NumberVariables = n_variables;
    bool generate_rand = seed != -1;
    LoadEdgeWeights(generate_rand, seed);
}

void FactorGraph::SetEdgeW(unsigned int clause, unsigned int variable, double value) {
    if (clause < this->NumberClauses && variable < this->NumberVariables)
        this->EdgeWeights[clause][variable] = value;
}

double FactorGraph::GetEdgeW(unsigned int clause, unsigned int variable) const {
    if (clause < this->NumberClauses && variable < this->NumberVariables)
        return this->EdgeWeights[clause][variable];
    else
        return 0.0;
}

void FactorGraph::ReadDIMACS(const std::string &path, int &n_clauses, int &n_variables) {
    std::ifstream input_file(path);
    std::string line;
    if (input_file.is_open()) {
        // Clear vectors.
        this->PositiveVariables.clear();
        this->NegativeVariables.clear();
        this->EdgeWeights.clear();
        this->PositiveClauses.clear();
        this->NegativeClauses.clear();

        // Skip the comments.
        while (getline(input_file, line) && line[0] == 'c');
        // Split the string
        std::vector<std::string> split = SplitString(line);
        // Check if the first line after the comments is the DIMACS header.
        if (split[0] == "p" && split[1] == "cnf") {
            n_variables = std::stoi(split[2]);
            n_clauses = std::stoi(split[3]);
            int counter = 0, actual_value = 0, i;
            // This are auxiliary vectors that stores the adjacency list of the actual clause
            std::vector<unsigned int> positive_adjacency_vector, negative_adjacency_vector;
            std::vector<unsigned int> *selected_variables_vector; // This pointer helps to not repeat code.
            // This are auxiliary vectors that stores the clauses where appear the actual variable
            std::vector<std::vector<unsigned int>> negative_clauses(n_variables);
            std::vector<std::vector<unsigned int>> positive_clauses(n_variables);
            std::vector<std::vector<unsigned int>> *selected_clauses_vector; // The same as the above pointer
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
                        if (actual_value > 0) {
                            selected_variables_vector = &positive_adjacency_vector;
                            selected_clauses_vector = &positive_clauses;
                        // If the actual value is positive we need to storage the info in the right vector
                        } else {
                            selected_variables_vector = &negative_adjacency_vector;
                            selected_clauses_vector = &negative_clauses;
                        }
                        // We save the information in the right vectors.
                        selected_variables_vector->push_back(abs(actual_value) - 1);
                        (*selected_clauses_vector)[abs(actual_value) - 1].push_back(counter);
                        i++;
                    }
                    //It's needed to push even when empty,
                    this->PositiveVariables.push_back(positive_adjacency_vector);
                    this->NegativeVariables.push_back(negative_adjacency_vector);

                    if (!positive_adjacency_vector.empty()) {
                        positive_adjacency_vector.clear();
                    }
                    if (!negative_adjacency_vector.empty()) {
                        negative_adjacency_vector.clear();
                    }
                    counter++;
                }
            }
            this->PositiveClauses = positive_clauses;
            this->NegativeClauses = negative_clauses;
        }else{
            std::cerr << "Enter a valid DIMACS file" << std::endl;
        }
    }else{
        std::cerr << "File not found" << std::endl;
    }
    std::cout << "File: " << path << " read correctly." << std::endl;
}

int FactorGraph::Connection(unsigned int clause, unsigned int variable, bool &positive) const {
    auto it = std::find(this->PositiveVariables[clause].cbegin(), this->PositiveVariables[clause].cend(),
                        variable - 1);
    auto pos = -1;
    // If the variable was founded, we have to find the index
    if(this->PositiveVariables[clause].cend() != it) {
        // The complexity of this function is constant if the iterators are random access iterators
        pos = std::distance(this->PositiveVariables[clause].cbegin(), it);
        positive = true;
    } else{
        it = std::find(this->NegativeVariables[clause].cbegin(), this->NegativeVariables[clause].cend(),
                       variable - 1);
        if(this->NegativeVariables[clause].cend() != it) {
            // The complexity of this function is constant if the iterators are random access iterators
            pos = std::distance(this->NegativeVariables[clause].cbegin(), it);
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
                this->EdgeWeights[i].push_back(value);
            } else {
                // If there is a negative connection between i clause and j variable
                if (pos != -1 && !positive) {
                    double value = rand ? distribution(generator) : 1.0;
                    this->EdgeWeights[i].push_back(-value);
                } else
                    this->EdgeWeights[i].push_back(0.0);
            }
        }
    }
}

FactorGraph FactorGraph::PartialAssignment(const std::vector<bool> &assignment) const {
    return FactorGraph();
}

std::vector<int> FactorGraph::Clause(unsigned int clause) const {
    std::vector<int> ret_clause;
    if (clause < this->NumberClauses) {
        ret_clause.insert(ret_clause.begin(), this->PositiveVariables[clause].begin(),
                          this->PositiveVariables[clause].end());

        for (auto it : this->NegativeVariables[clause])
            ret_clause.push_back(-it);
    }
    return ret_clause;
}

std::vector<unsigned int> FactorGraph::ClausesOfVariable(unsigned int variable) const {
    std::vector<unsigned int> ret_clause;
    if (variable < this->NumberVariables) {
        ret_clause = this->PositiveClauses[variable];

        for (auto it : this->NegativeClauses[variable])
            ret_clause.push_back(it);
    }
    return ret_clause;
}

void
FactorGraph::VariablesInClause(unsigned int clause, std::vector<int> &positives, std::vector<int> &negatives) const {
    if (!positives.empty())
        positives.clear();

    if (!negatives.empty())
        negatives.clear();

    positives.insert(positives.begin(), this->PositiveVariables[clause].cbegin(),
                     this->PositiveVariables[clause].cend());
    negatives.insert(negatives.begin(), this->NegativeVariables[clause].cbegin(),
                     this->NegativeVariables[clause].cend());
}

std::ostream &operator<<(std::ostream &out, const FactorGraph &graph) {
    out << "p cnf " << graph.NumberVariables << " " << graph.NumberClauses << std::endl;
    for (int i = 0; i < graph.PositiveVariables.size(); i++) {
        for (int it : graph.Clause(i)) {
            if (it >= 0)
                out << it + 1 << " ";
            else
                out << it - 1 << " ";
        }
        out << "0" << std::endl;
    }
    return out;
}

std::vector<std::string> SplitString(const std::string &str, char delim) {
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
