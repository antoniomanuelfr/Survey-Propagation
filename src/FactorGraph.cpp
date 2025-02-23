//
// Created by antoniomanuelfr on 3/25/20.
//

#include "FactorGraph.h"

std::ostream &operator << (std::ostream &out, const clause &clause) {
    for (auto i : clause) {
        out << i << " " ;
    }
    out << "0" << std::endl;
    return out;
}

vector<std::string> SplitString(const std::string &str, char delim) {
    if (str.empty()) {
        return vector<std::string>();
    }
    // Vector with the sub-strings
    vector<std::string> cont;
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

uvector genIndexVector(unsigned int N) {
    uvector ordered_indexes;
    ordered_indexes.resize(N);
    std::iota(ordered_indexes.begin(), ordered_indexes.end(), 0);

    return ordered_indexes;
}

FactorGraph::FactorGraph(const std::string &path, int seed) {
    int n_clauses = 0, n_variables = 0;
    this->seed = seed;
    ReadDIMACS(path, n_clauses, n_variables);
    this->NumberClauses = n_clauses;
    this->NumberVariables = n_variables;
    ChangeWeights();
}

void FactorGraph::getUnitVars(std::unordered_map<unsigned int, bool> &unit_vars) const {
    unsigned int variable;
    bool variable_assignment;
    for (int i = 0; i < this->NumberClauses; i++) {
        variable = 0;
        int pos_size = this->PositiveVariablesOfClause[i].size(), neg_size = this->NegativeVariablesOfClause[i].size();
        if (pos_size + neg_size == 1) {
            if (pos_size == 1) {
                variable = this->PositiveVariablesOfClause[i][0];
                variable_assignment = true;
            } else {
                variable = this->NegativeVariablesOfClause[i][0];
                variable_assignment = false;
            }
        }
        if (variable != 0 ) {
            if (unit_vars.find(variable) == unit_vars.end()) {
                unit_vars.insert(std::pair<unsigned int, bool> (variable, variable_assignment));
            }
        }
    }
}

unsigned int FactorGraph::getIndexOfVariable(unsigned int search_clause, int variable) const {
    bool connection_type;
    unsigned int index = this->Connection(search_clause, abs(variable), connection_type);
    /* Get the index of variable in search_clause. In the clauses (and weights), first appear the positive variables and
     * then appear the negative, so in order to get the index: See if the connection is negative. If it is
     * negative adding de number of positive variables in the clause we get the index. */
    if (index == -1) {
        exit(1);
    }
    index += connection_type ? 0 : this->getPositiveVariablesOfClause(search_clause).size();

    return index;
}

void FactorGraph::setEdgeW(unsigned int search_clause, unsigned int position, double value) {
    if (search_clause < this->NumberClauses && position < this->NumberVariables) {
        this->EdgeWeights[search_clause][position] = value;
    } else {
        exit(1);
    }
}

double FactorGraph::getEdgeW(unsigned int search_clause, unsigned int position) const {
    if (search_clause < this->NumberClauses && position < this->NumberVariables)
        return this->EdgeWeights[search_clause][position];
    else {
        std::cerr << "Wrong index!!" << std::endl;
        exit(1);
    }
}

void FactorGraph::ReadDIMACS(const std::string &path, int &n_clauses, int &n_variables) {
    std::ifstream input_file(path);
    std::string line;
    if (input_file.is_open()) {
        // Clear vectors.
        this->PositiveVariablesOfClause.clear();
        this->NegativeVariablesOfClause.clear();
        this->EdgeWeights.clear();
        this->PositiveClausesOfVariable.clear();
        this->NegativeClausesOfVariable.clear();

        // Skip the comments.
        while (getline(input_file, line) && line[0] == 'c');
        // Split the string
        vector<std::string> split = SplitString(line);

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
                    this->PositiveVariablesOfClause.push_back(positive_adjacency_vector);
                    this->NegativeVariablesOfClause.push_back(negative_adjacency_vector);

                    if (!positive_adjacency_vector.empty()) {
                        positive_adjacency_vector.clear();
                    }
                    if (!negative_adjacency_vector.empty()) {
                        negative_adjacency_vector.clear();
                    }
                    counter++;
                }
            }
            this->PositiveClausesOfVariable = positive_variables;
            this->NegativeClausesOfVariable = negative_variables;
        }else{
            std::cerr << "Enter a valid DIMACS file" << std::endl;
            exit(-1);
        }
    }else{
        std::cerr << "File not found" << std::endl;
        exit(-1);
    }
    input_file.close();
}

int FactorGraph::Connection(unsigned int search_clause, unsigned int variable, bool &positive) const {
    auto it = std::find(this->PositiveVariablesOfClause[search_clause].cbegin(),
                        this->PositiveVariablesOfClause[search_clause].cend(), variable);
    auto pos = -1;
    // If the variable was founded, we have to find the index
    if (this->PositiveVariablesOfClause[search_clause].cend() != it) {
        // The complexity of this function is constant if the iterators are random access iterators
        pos = std::distance(this->PositiveVariablesOfClause[search_clause].cbegin(), it);
        positive = true;
    } else {
        it = std::find(this->NegativeVariablesOfClause[search_clause].cbegin(),
                       this->NegativeVariablesOfClause[search_clause].cend(), variable);
        if (this->NegativeVariablesOfClause[search_clause].cend() != it) {
            // The complexity of this function is constant if the iterators are random access iterators
            pos = std::distance(this->NegativeVariablesOfClause[search_clause].cbegin(), it);
            positive = false;
        }
    }
    return pos;
}

void FactorGraph::ChangeWeights() {
    if (!this->EdgeWeights.empty())
        this->EdgeWeights.clear();

    std::mt19937 generator(this->seed); // Random engine generator.
    std::uniform_real_distribution<double> distribution(0, 1); //Distribution for the random generator.
    // Reserve memory
    this->EdgeWeights.resize(this->NumberClauses);
    clause actual_clause;
    for(int i = 0; i < this->NumberClauses; i++) {
        actual_clause = this->Clause(i);
        for (int j = 0; j < actual_clause.size(); j++) {
            this->EdgeWeights[i].push_back(distribution(generator));
        }
    }
}

void FactorGraph::UnitPropagation() {
    std::unordered_map<unsigned int, bool> unit_vars;

    this->getUnitVars(unit_vars);
    auto it = unit_vars.begin();

    do {
        while (it != unit_vars.end()) {
            this->PartialAssignment(it->first - 1, it->second);
            unit_vars.erase(it->first);
            it = unit_vars.begin();
        }
        this->getUnitVars(unit_vars);
        it = unit_vars.begin();
    } while (!unit_vars.empty());
}

void FactorGraph::ApplyNewClauses(const vector<vector<int>> &deleted, const vector<bool> &satisfied) {
    if (this->NumberClauses != 0) {
        unsigned int del = 0, actual_clause;
        uvector *selected_clauses, *selected_variables;
        clause cl;
        for (int clause = 0; clause < this->NumberClauses; clause++) {
            actual_clause = clause - del;
            // If the clause is not satisfied, remove the variables assigned.
            if (!satisfied[clause]) {
                for (int variable : deleted[clause]) {
                    // Selection of the positive vectors.
                    selected_clauses = variable > 0 ? &this->PositiveVariablesOfClause[actual_clause] :
                                       &this->NegativeVariablesOfClause[actual_clause];
                    // Find the variable to delete in the correct clause.
                    auto it = std::find(selected_clauses->begin(), selected_clauses->end(), abs(variable));
                    // Calculate the position of the variable in the clause in order to delete it from EdgeWeights.
                    unsigned int diff = std::distance(selected_clauses->begin(), it) +
                                        (variable > 0 ? 0 : this->PositiveVariablesOfClause[actual_clause].size());
                    selected_clauses->erase(it);
                    // We delete the weight of the edge.
                    this->EdgeWeights[actual_clause].erase(this->EdgeWeights[actual_clause].begin() + diff);
                }
                // If the clause if satisfied, remove it.
            } else {
                this->PositiveVariablesOfClause.erase(this->PositiveVariablesOfClause.begin() + (actual_clause));
                this->NegativeVariablesOfClause.erase(this->NegativeVariablesOfClause.begin() + (actual_clause));
                this->EdgeWeights.erase(EdgeWeights.begin() + actual_clause);
                del++;
            }
        }
        // Make the Variables vectors with the new assignment.
        for (auto &v : this->PositiveClausesOfVariable) {
            v.clear();
        }
        for (auto &v : this->NegativeClausesOfVariable) {
            v.clear();
        }

        for (int clause = 0; clause < this->PositiveVariablesOfClause.size(); clause++) {
            cl = this->Clause(clause);
            for (auto var : cl) {
                selected_variables =
                        var > 0 ? &this->PositiveClausesOfVariable[var - 1] : &this->NegativeClausesOfVariable[abs(var) - 1];
                selected_variables->push_back(clause);
            }
        }
        this->NumberClauses = this->PositiveVariablesOfClause.size();
    }
}

void FactorGraph::PartialAssignment(unsigned int variable_index, bool assignation) {
    int variable = static_cast<int>(variable_index) + 1;
    vector<bool> satisfied_clauses(this->NumberClauses, false);
    // This matrix will save the deleted variables from each clause.
    vector<vector<int>> deleted_variables_from_clauses;
    deleted_variables_from_clauses.resize(this->NumberClauses);
    // Positive variables.
    for (auto search_clause : this->PositiveClausesOfVariable[variable_index]) {
        // If the variable appears as positive and the assignment is false, the variable is deleted from the clause.
        if (!assignation) {
            deleted_variables_from_clauses[search_clause].push_back(variable);
        // If the variable appears as positive and the assignment is true, the clause is satisfied
        } else {
            satisfied_clauses[search_clause] = true;
        }
    }
    // Negative variables.
    for (auto search_clause : this->NegativeClausesOfVariable[variable_index]) {
        // If the variable appears as negative and the assignment is true, the variable is deleted from the clause.
        if (assignation) {
            deleted_variables_from_clauses[search_clause].push_back(-variable);
        // If the variable appears as negative and the assignment is false, the clause is satisfied.
        } else {
            satisfied_clauses[search_clause] = true;
        }
    }
    this->ApplyNewClauses(deleted_variables_from_clauses, satisfied_clauses);
}

clause FactorGraph::Clause(unsigned int search_clause) const {
    clause ret_clause;
    if (search_clause < this->NumberClauses) {
        ret_clause.reserve(this->PositiveVariablesOfClause[search_clause].size() + this->NegativeVariablesOfClause[search_clause].size());
        for (auto it : this->PositiveVariablesOfClause[search_clause])
            ret_clause.push_back(it);
        for (auto it : this->NegativeVariablesOfClause[search_clause])
                ret_clause.push_back(-it);
    }
    return ret_clause;
}

uvector FactorGraph::getClausesOfVariable(int variable) const {
    uvector ret_clause;
    unsigned int variable_index = variable > 0 ? variable - 1 : abs(variable) - 1;
        if (variable_index < this->NumberVariables) {
        ret_clause.reserve(this->PositiveClausesOfVariable[variable_index].size() + this->NegativeClausesOfVariable[variable_index].size());

        ret_clause = this->PositiveClausesOfVariable[variable_index];
        for (auto it : this->NegativeClausesOfVariable[variable_index])
            ret_clause.push_back(it);

    } else {
        exit(1);
    }
    return ret_clause;
}

bool FactorGraph::SatisfiesC(const vector<bool> &assignment, const clause &search_clause) {
    int index;
    for (int i : search_clause) {
        index = i > 0 ? i - 1 : abs(i) - 1;
        if (((!assignment[index] && i < 0) || (assignment[index] && i > 0))) {
            return true;
        }
    }
    return false;
}

bool FactorGraph::SatisfiesF(const vector<bool> &assign, vector<bool> &sat, const uvector &indexes) const {

    bool sat_clause;
    bool satisfied_formula = true;
    for (auto search_clause : indexes) {
        sat_clause = this->SatisfiesC(assign, this->Clause(search_clause));
        if (!sat_clause) {
            satisfied_formula = false;
        }
        sat[search_clause] = sat_clause;
    }
    return satisfied_formula;
}

uvector FactorGraph::getBreakCount(const vector<bool> &sat_clauses, const clause &s_clause, const vector<bool> &assign,
                                   unsigned int &min_index, int &freebie) const {

    uvector break_count(s_clause.size(), 0);
    uvector sat_count(s_clause.size(), 0);
    vector<bool> copy(assign);
    clause c;

    unsigned int variable_index;
    unsigned int n_sat_clauses = 0;

    unsigned int min = this->NumberClauses;
    freebie = -1;

    for (int i = 0; i < s_clause.size(); i++) {
        n_sat_clauses = 0;
        variable_index = s_clause[i] > 0 ? s_clause[i] - 1 : abs(s_clause[i]) - 1;
        // Get the clauses where each variable appears.
        for (auto clause_index : this->getClausesOfVariable(s_clause[i])) {
            // If the clause is satisfied, we calculate the break count of that variable
            if (sat_clauses[clause_index]) {
                // flip the variable
                copy[variable_index] = !copy[variable_index];
                // check the assignment
                if (!this->SatisfiesC(copy, this->Clause(clause_index))) {
                    break_count[i]++;
                } else {
                    sat_count[i]++;
                }
                // Restore the assignment
                copy[variable_index] = !copy[variable_index];
                n_sat_clauses++;
            }
        }

        if (break_count[i] < min) {
            min = break_count[i];
            min_index = i;
        }

        if (sat_count[i] == n_sat_clauses) {
            freebie = i;
        }
    }
    return break_count;
}

vector<bool>
FactorGraph::WalkSAT(unsigned int max_tries, unsigned int max_flips, double noise, const vector<int>& fixed_variables) const {

    unsigned int min_index, v;
    bool sat;
    int freebie = -1;

    vector<bool> assignment(this->NumberVariables);
    vector<bool> sat_clauses(this->NumberClauses, false);
    uvector not_satisfied_clauses, indexes(this->NumberClauses);

    std::mt19937 gen(this->seed * 2); // Random engine generator.
    std::uniform_int_distribution<int> bdist(0, 1); //Distribution for the random boolean generator.
    std::uniform_real_distribution<double> double_dist(0, 1); //Distribution for the random real generator.

    for (int i = 0; i < max_tries; i++) {
        std::generate(assignment.begin(), assignment.end(), [&bdist, &gen]() {return static_cast<bool>(bdist(gen));});
        indexes.clear();
        indexes = genIndexVector(this->NumberClauses);
        // Update the sat_clauses vector of the clauses that where changed previously.
        for (int flips = 0; flips < max_flips; flips++) {
            this->SatisfiesF(assignment, sat_clauses, indexes);
            // Get the clauses state of the clauses with the given assignment.
            not_satisfied_clauses.clear();
            sat = true;
            // Check if the assignment satisfies the formula. Save the not satisfied clauses in an auxiliary vector.
            for (int clause = 0; clause < sat_clauses.size(); clause++) {
                if (!sat_clauses[clause]) {
                    sat = false;
                    not_satisfied_clauses.push_back(clause);
                }
            }
            // If the formula is satisfied, return the assignment.
            if (sat) {
                return assignment;
            }

            std::uniform_int_distribution<int> int_dist(0, not_satisfied_clauses.size() - 1);
            // We get a random not satisfied clause
            clause C = this->Clause(not_satisfied_clauses[int_dist(gen)]);
            uvector count = this->getBreakCount(sat_clauses, C, assignment, min_index, freebie);
            // Check the freebie move.
            if (freebie != -1) {
                v = freebie;
            } else if (double_dist(gen) > noise) {
                std::uniform_int_distribution<unsigned int> dis(0, C.size() - 1);
                v = dis(gen);
            // We choose tha variable with lower break count
            } else {
                v = min_index;
            }
            // Get the index of the variable that will be flipped
            int index = C[v] > 0 ? C[v] - 1 : abs(C[v]) - 1;
            assignment[index] = !assignment[index];
            // Check if the new assignment satisfies the clauses where the selected variable appears.
            // We update the variables that are going to be searched
            indexes = this->getClausesOfVariable(C[v]);
        }
    }
    return vector<bool>();
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

bool FactorGraph::CheckAssignment(const vector<bool> &assignment) const {
    if (assignment.size() != this->NumberVariables) {
        std::cerr << "Assignment vector is invalid" << std::endl;
        return false;
    }

    for (int clause = 0; clause < this->NumberClauses; clause++) {
        if (!this->SatisfiesC(assignment, this->Clause(clause))){
            return false;
        }
    }
    return true;
}
