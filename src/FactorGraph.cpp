//
// Created by antoniomanuelfr on 3/25/20.
//

#include "FactorGraph.h"

bool operator == (const clause &c1, const clause &c2) {
    if (c1.size() != c2.size()) {
        return false;
    }
    for (int i = 0; i < c1.size(); i++) {
        if (c1[i] != c2[i]) {
            return false;
        }
    }
    return true;
}

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

FactorGraph::FactorGraph(const std::string &path, int seed) {
    int n_clauses = 0, n_variables = 0;
    ReadDIMACS(path, n_clauses, n_variables);
    this->NumberClauses = n_clauses;
    this->NumberVariables = n_variables;
    bool generate_rand = seed != -1;
    RandomizeWeights(generate_rand, seed);
}

uvector FactorGraph::getUnitVars() const {
    uvector unit_vars;
    for (int i = 0; i < this->NumberClauses; i++) {
        int pos_size = this->PositiveClauses[i].size(), neg_size = this->NegativeClauses[i].size();
        if (pos_size + neg_size == 1) {
            if (pos_size == 1) {
                unit_vars.push_back(this->PositiveClauses[i][0]);
            } else {
                unit_vars.push_back(this->NegativeClauses[i][0]);
            }
        }
    }
    return unit_vars;
}

void FactorGraph::setEdgeW(unsigned int search_clause, unsigned int variable, double value) {
    if (search_clause < this->NumberClauses && variable < this->NumberVariables) {
        this->EdgeWeights[search_clause][variable] = value;
    }
}

double FactorGraph::getEdgeW(unsigned int search_clause, unsigned int variable) const {
    if (search_clause < this->NumberClauses && variable < this->NumberVariables)
        return this->EdgeWeights[search_clause][variable];
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
            exit(-1);
        }
    }else{
        std::cerr << "File not found" << std::endl;
        exit(-1);
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

void FactorGraph::RandomizeWeights(bool rand, unsigned long seed) {
    if (!this->EdgeWeights.empty())
        this->EdgeWeights.clear();

    std::default_random_engine generator(seed); // Random engine generator.
    std::uniform_real_distribution<double> distribution(0,1); //Distribution for the random generator.
    // Reserve memory
    this->EdgeWeights.resize(this->NumberClauses);
    clause actual_clause;
    for(int i = 0; i < this->NumberClauses; i++) {
        actual_clause = this->Clause(i);
        for (int j = 0; j < actual_clause.size(); j++) {
            this->EdgeWeights[i].push_back(rand ? distribution(generator) : 1.0);
        }
    }
}

void FactorGraph::UnitPropagation() {
    uvector unit_vars = this->getUnitVars();
    for (auto unit_var : unit_vars) {
        this->PartialAssignment(unit_var - 1, unit_var > 0);
    }
}

void FactorGraph::ApplyNewClauses(const std::vector<std::vector<int>> &deleted, const std::vector<bool> &satisfied) {
    if (this->NumberClauses != 0) {
        unsigned int del = 0, actual_clause;
        uvector *selected_clauses, *selected_variables;
        clause cl;
        for (int clause = 0; clause < deleted.size(); clause++) {
            actual_clause = clause - del;
            // If the clause is not satisfied, remove the variables assigned.
            if (!satisfied[clause]) {
                for (int variable : deleted[clause]) {
                    // Selection of the positive vectors.
                    selected_clauses = variable > 0 ? &this->PositiveClauses[actual_clause] :
                                       &this->NegativeClauses[actual_clause];
                    // Find the variable to delete in the correct clause.
                    auto it = std::find(selected_clauses->begin(), selected_clauses->end(), abs(variable));
                    selected_clauses->erase(it);
                    unsigned int diff = std::distance(selected_clauses->begin(), it) +
                            (variable > 0 ? 0 : this->PositiveClauses[actual_clause].size());
                    // We delete the weight of the edge.
                    this->EdgeWeights[actual_clause].erase(this->EdgeWeights[actual_clause].begin() + diff);
                }
                // If the clause if satisfied, remove it.
            } else {
                this->PositiveClauses.erase(this->PositiveClauses.begin() + (actual_clause));
                this->NegativeClauses.erase(this->NegativeClauses.begin() + (actual_clause));
                this->EdgeWeights.erase(EdgeWeights.begin() + actual_clause);
                del++;

            }
        }
        // Make the Variables vectors with the new assignment.
        for (auto &v : this->PositiveVariables) {
            v.clear();
        }
        for (auto &v : this->NegativeVariables) {
            v.clear();
        }

        for (int clause = 0; clause < this->PositiveClauses.size(); clause++) {
            cl = this->Clause(clause);
            for (auto var : cl) {
                selected_variables =
                        var > 0 ? &this->PositiveVariables[var - 1] : &this->NegativeVariables[abs(var) - 1];
                selected_variables->push_back(clause);
            }
        }
        this->NumberClauses = this->PositiveClauses.size();
    }
}

void FactorGraph::PartialAssignment(unsigned int variable, bool assignation) {
    bool type;
    int index;
    std::vector<bool> satisfied_clauses(this->NumberClauses, false);
    // This matrix
    std::vector<std::vector<int>> deleted_variables_from_clauses;
    deleted_variables_from_clauses.resize(this->NumberClauses);
        // If the variable i changes.
    for (int search_clause = 0; search_clause < this->NumberClauses; search_clause++) {
        index = this->Connection(search_clause, variable + 1, type);
        // If the variable i is in search_clause.
        if (index != -1) {
            // If the variable appears as negative and we assign it to true.
            if (!type && assignation) {
                deleted_variables_from_clauses[search_clause].push_back(-(variable + 1));
                // If the variable appears as positive and we assign it to negative.
            } else if (type && !assignation) {
                deleted_variables_from_clauses[search_clause].push_back(variable + 1);
            }
                // If the variable appears as positive and we assign it to positive or
                // If the variable appears as negative and we assign it to negative,
                // the clause will be satisfied and deleted.
            else {
                satisfied_clauses[search_clause] = true;
            }
        }
    }
    this->ApplyNewClauses(deleted_variables_from_clauses, satisfied_clauses);
}

clause FactorGraph::Clause(unsigned int search_clause) const {
    clause ret_clause;
    if (search_clause < this->NumberClauses) {
        ret_clause.reserve(this->PositiveClauses[search_clause].size() + this->NegativeClauses[search_clause].size());
        for (auto it : this->PositiveClauses[search_clause])
            ret_clause.push_back(it);
        for (auto it : this->NegativeClauses[search_clause])
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

void FactorGraph::ClausesInVariable(unsigned int variable_index, uvector &positive, uvector &negatives) const {
    if (!positive.empty())
        positive.clear();

    if (!negatives.empty())
        negatives.clear();

    positive.insert(positive.begin(), this->PositiveVariables[variable_index].cbegin(),
                     this->PositiveVariables[variable_index].cend());
    negatives.insert(negatives.begin(), this->NegativeVariables[variable_index].cbegin(),
                     this->NegativeVariables[variable_index].cend());
}

bool FactorGraph::SatisfiesC(const std::vector<bool> &assignment, const clause &search_clause) const {
    int index;
    for (int i : search_clause) {
        index = i > 0 ? i - 1 : abs(i) - 1;
        if (((!assignment[index] && i < 0) || (assignment[index] && i > 0))) {
            return true;
        }
    }
    return false;
}

bool FactorGraph::SatisfiesF(const std::vector<bool> &assign, uvector &n_sat_clauses, uvector &sat_clauses, uvector &indxs) const {

    if (!n_sat_clauses.empty()) {
        n_sat_clauses.clear();
    }
    if (!sat_clauses.empty()) {
        sat_clauses.clear();
    }

    bool satisfied_formula = true;
    for (auto search_clause : indxs) {
        if (!this->SatisfiesC(assign, this->Clause(search_clause))) {
            satisfied_formula = false;
            n_sat_clauses.push_back(search_clause);
        } else {
            sat_clauses.push_back(search_clause);
        }
    }
    return satisfied_formula;
}

uvector FactorGraph::getBreakCount(const uvector &sat_clauses, const clause &s_clause, const std::vector<bool> &assign) const {

    uvector break_count(s_clause.size(), 0);
    std::vector<bool> sat_vars;
    int index;
    clause c;
    for (auto clause_index : sat_clauses) {
        c = this->Clause(clause_index);
        for (int i = 0; i < s_clause.size(); i++) {
            index = s_clause[i] > 0 ? s_clause[i] - 1 : abs(s_clause[i]) - 1;
            // If flipping the variable var causes that the clause won't be satisfied, break_count[var]++
            if (!((assign[index] && c[i] < 0) || (!assign[index] && c[i] > 0))) {
                break_count[i]++;
            }
        }
    }
    return break_count;
}

// TODO: Improve WalkSAT implementation.
std::vector<bool> FactorGraph::WalkSAT(unsigned int max_tries, unsigned int max_flips, double noise, int seed) const {
    int v;
    std::vector<bool> assignment(this->NumberVariables);
    uvector not_satisfied_clauses, satisfied_clauses, indexes(this->NumberClauses);
    std::default_random_engine gen(seed); // Random engine generator.
    std::uniform_int_distribution<int> bdist(0, 1); //Distribution for the random boolean generator.
    std::uniform_real_distribution<double> double_dist(0, 1); //Distribution for the random real generator.

    for (int i = 0; i < max_tries; i++) {
        std::generate(assignment.begin(), assignment.end(), [&bdist, &gen]() { return static_cast<bool>(bdist(gen)); });
        std::iota(indexes.begin(), indexes.end(), 0);

        for (int flips = 0; flips < max_flips; flips++) {
            if (this->SatisfiesF(assignment, not_satisfied_clauses, satisfied_clauses, indexes)) {
                return assignment;
            }
            std::uniform_int_distribution<int> int_dist(0, not_satisfied_clauses.size() - 1);
            // We get a random not satisfied clause
            clause C = this->Clause(not_satisfied_clauses[int_dist(gen)]);
            uvector count = this->getBreakCount(satisfied_clauses, C, assignment);
            // Check if there is any variable of C with break count equal to 0.
            bool find = false;
            for (int j = 0; j < count.size(); j++) {
                if (count[j]) {
                    find = true;
                    v = j;
                    break;
                }
            }
            // If there is not any variable with break count equal to zero
            if (!find) {
                // We choose a random v of C
                if (double_dist(gen) > noise) {
                    std::uniform_int_distribution<int> dis(0, C.size() - 1);
                    v = dis(gen);
                // We choose tha variable with lower break count
                } else {
                    v = std::distance(count.begin(), std::min_element(count.begin(), count.end()));
                }
            }
            int index = C[v] > 0 ? C[v] - 1 : abs(C[v]) - 1;
            assignment[index] = !assignment[index];
        }

    }
    return std::vector<bool>();
}

std::ostream &operator << (std::ostream &out, const FactorGraph &graph) {
    out << "p cnf " << graph.NumberVariables << " " << graph.NumberClauses << std::endl;
    for (int i = 0; i < graph.NumberClauses; i++) {
        for (int it : graph.Clause(i)) {
            out << it << " ";
        }
        out << "0" << std::endl;
    }
    return out;
}

std::ostream &operator << (std::ostream &out, const clause &clause) {
    for (auto i : clause) {
        out << i << " " ;
    }
    out << "0" << std::endl;
    return out;
}

std::vector<std::string> SplitString(const std::string &str, char delim) {
    if (str.empty()) {
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
