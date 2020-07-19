//
// Created by antoniomanuelfr on 3/25/20.
//

#ifndef FACTOR_GRAPH_H
#define FACTOR_GRAPH_H

#include <vector>
#include <utility>
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <random>
#include <unordered_map>

using std::vector;

/** Create definition for uvector. Stands for an unsigned vector of the standard library. */
typedef vector<unsigned int> uvector;
/** Create definition for clause. Stands for an int vector of the standard library. */
typedef vector<int> clause;
/** Create definition for to umatrix. Stands for a unsigned int matrix of standard library. */
typedef vector<vector<unsigned int>> umatrix;
/** Create definition for wmatrix. Stands for a double matrix of the standard library. */
typedef vector<vector<double>> wmatrix;

/**
 * @brief Operator == for two clauses.
 * @param c1: First clause.
 * @param c2: Second clause.
 * @return True if the two clauses are equal (element by element).
 */
bool operator == (const clause &c1, const clause &c2);

/**
 * @brief Operator << overload for clause. The output will have DIMACS syntax
 * @param out: Ostream object (can be a file, standard output).
 * @param clause: Clause vector that is going to be printed out.
 * @return Ostream object with the contents of clause.
 */
std::ostream &operator << (std::ostream &out, const clause &clause);

/**
 * @brief Split a string separated by a delimiter.
 * @param str: String that is going to be splitted.
 * @param delim: Delimiter. By default is an space
 * @return A vector<string> with the splits of the string.
 */
vector<std::string> SplitString(const std::string& str, char delim = ' ');

/**
 * @brief Class for handle the factor graph representation of a CNF formula.
 *  The variables in a DIMACS file are in the range [1,NumberVariables]
 *  and the 0 variable means that the clause has finished. The representation that is used goes from
 *  [0,NumberVariables). So when we are saying that PositiveClausesOfVariable[0] we are getting the clauses where the
 *  variable 1 is positive.
 **/

[[nodiscard]] uvector genIndexVector(unsigned int N);

class FactorGraph {

private:

    /** Vector that storage the positive variables of each clause. */
    umatrix PositiveVariablesOfClause;
    /** Vector that storage the negative variables of each clause. */
    umatrix NegativeVariablesOfClause;
    /** Vector that storage for each variable the clauses where that variable appear as positive. */
    umatrix PositiveClausesOfVariable;
    /** Vector that storage for each variable the clauses where that variable appear as negative. */
    umatrix NegativeClausesOfVariable;
    /** Vector that storage the edge pis (weight). */
    wmatrix EdgeWeights;
    /** Variable that storage the number of clauses. */
    int NumberClauses{0};
    /** Variable that storage the number of variables. */
    int NumberVariables{0};
    /** Seed for the RNG */
    int seed{0};

    /**
     * @brief Read a DIMACS file (the clauses of the DIMACS file must be in conjunctive normal form).
     * and overwrite the Positive and Negative vectors by it's content.
     * @param path: Path to the file.
     * @param n_clauses: int where the number of clauses that was founded will be stored.
     * @param n_variables: int where the number of variables that was founded will be stored.
     */
    void ReadDIMACS(const std::string &path, int &n_clauses, int &n_variables);

    /**
     * @brief Function that loads new clauses vectors. It is used in the PartialAssignment function.
     * @param deleted: Matrix where each row is the clause where the variable of each column will be deleted.
     * @param satisfied: Vector with the clauses that are satisfied.
     */
    void ApplyNewClauses(const vector<vector<int>> &deleted, const vector<bool> &satisfied);

public:

    /**
     * @brief Empty constructor.
     */
    FactorGraph() = default;

    /**
     * @brief Copy constructor for FactorGraph class.
     * @param fc: Factor graph to copy.
     */
    [[maybe_unused]] FactorGraph(const FactorGraph &fc) {
        this->PositiveClausesOfVariable.clear();
        this->NegativeClausesOfVariable.clear();
        this->PositiveVariablesOfClause.clear();
        this->NegativeVariablesOfClause.clear();
        this->EdgeWeights.clear();

        this->PositiveVariablesOfClause = fc.PositiveVariablesOfClause;
        this->NegativeVariablesOfClause = fc.NegativeVariablesOfClause;
        this->PositiveClausesOfVariable = fc.PositiveClausesOfVariable;
        this->NegativeClausesOfVariable = fc.NegativeClausesOfVariable;
        this->EdgeWeights = fc.EdgeWeights;
        this->seed = fc.seed;

        this->NumberClauses = fc.NumberClauses;
        this->NumberVariables = fc.NumberVariables;
    }

    /**
     * @brief Constructor for FactorGraph.
     * @param path: DIMACS file path.
     * @param seed: Seed that will be used. Defaults to 0.
     */
    explicit FactorGraph(const std::string &path, int seed = 0);

    /**
     * @brief Getter for NumberClauses.
     * @return Integer with the value of NumberClauses. If the output of this function is discarded,
     * the compiler will raise a warning.
     */
    [[nodiscard]] int getNClauses() const {
        return NumberClauses;
    }

    /**
     * @brief Getter for NumberVariables.
     * @return Integer with the value of NumberVariables. If the output of this function is discarded,
     * the compiler will raise a warning.
     */
    [[nodiscard]] int getNVariables() const {
        return NumberVariables;
    }

    /**
     * @brief Check if the formula is an empty clause.
     * @return True if the formula is an empty clause or false if not. If the output of this function is discarded,
     * the compiler will raise a warning.
     */
    [[nodiscard]] bool EmptyClause() const {
        return this->NumberClauses == 0;
    }

    /**
    * @brief Check if the formula is a contradiction.
    * @return True if the formula is a contradiction or false if not. If the output of this function is discarded,
    * the compiler will raise a warning.
    */
    [[nodiscard]] bool Contradiction() const {
        for (int i = 0 ; i < this->NumberClauses; i++) {
            if (this->Clause(i).empty()) {
                return true;
            }
        }
        return false;
    }

    /**
     * @brief Getter for a specific edge weight.
     * @param search_clause: Clause of the edge.
     * @param variable: Position where the variable appears.
     * @return The assigned weight of the edge between search_clause and variable. If the output of this function is
     * discarded, the compiler will raise a warning.
     */
    [[nodiscard]] double getEdgeW(unsigned int search_clause, unsigned int position) const;

    /**
     * @brief Function that return the weights matrix.
     * @return A const reference to EdgeWeights. If the output of this function is discarded,
     * the compiler will raise a warning.
     */
    [[nodiscard]] const wmatrix & getMatrix() const {
        return this->EdgeWeights;
    }

     /**
     * @brief Getter for PositiveVariablesOfClause vector.
     * @param search_clause: Clause to look for.
     * @return A const reference to PositiveVariablesOfClauses. If the output of this function is discarded,
     * the compiler will raise a warning.
     */
    [[nodiscard]] const uvector & getPositiveVariablesOfClause(unsigned int search_clause) const {
        return this->PositiveVariablesOfClause[search_clause];
    }

    /**
     * @brief Getter for NegativeVariablesOfClause.
     * @param search_clause: Clause to look for.
     * @return A const reference to NegativeVariablesOfClause. If the output of this function is discarded,
     * the compiler will raise a warning.
     */
    [[nodiscard]] const uvector & getNegativeVariablesOfClause(unsigned int search_clause) const {
        return this->NegativeVariablesOfClause[search_clause];
    }

    /**
     * @brief Getter for PositiveClausesOfVariable.
     * @param variable: Variable to look for.
     * @return A const reference to PositiveClausesOfVariable. If the output of this function is discarded,
     * the compiler will raise a warning.
     */
    [[nodiscard]] const uvector & getPositiveClausesOfVariable(int variable) const {
        unsigned int variable_index = variable > 0 ? variable - 1 : abs(variable) - 1;
        return this->PositiveClausesOfVariable[variable_index];
    }

    /**
     * @brief Getter for NegativeClausesOfVariable.
     * @param variable: Variable to look for.
     * @return A const reference to NegativeClausesOfVariable. If the output of this function is discarded,
     * the compiler will raise a warning.
     */
    [[nodiscard]] const uvector & getNegativeClausesOfVariable(int variable) const {
        unsigned int variable_index = variable > 0 ? variable - 1 : abs(variable) - 1;
        return this->NegativeClausesOfVariable[variable_index];
    }

    /**
     * @brief Function that returns the unit vars.
     * @return A vector with the unit variables of the formula. If the output of this function is discarded,
     * the compiler will raise a warning.
     */
    void getUnitVars(std::unordered_map<unsigned int, bool> &unit_vars) const;

    /**
     * @brief Get the index of variable in search_clause
     * @param search_clause: Clause to look for.
     * @param variable: Variable to look for.
     * @return The index of the variable in search_clause. If an the variable doesn't appear in the clause, the
     * execution will be aborted with the exit_status = 1;
     */
    [[nodiscard]] unsigned int getIndexOfVariable(unsigned int search_clause, int variable) const;

    /**
     * @brief Change the weight of a specific edge.
     * @param search_clause: Clause of the edge.
     * @param position: Position of the variable inside the EdgeWeights vector.
     * @param value: New weight of the edge.
     */
    void setEdgeW(unsigned int search_clause, unsigned int position, double value);

    /**
     * @brief Check if a variable appears in a search_clause.
     * @param search_clause: Clause where to look for the variable.
     * @param variable: Variable that will be looked in Clause.
     * @param positive: Will be true if the variable was founded in positives nodes, false in other case.
     * @return Index of the variable in the search_clause. If not found the return value is -1. If the output of this
     * function is discarded, the compiler will raise a warning.
     */
    [[nodiscard]] int Connection(unsigned int search_clause, unsigned int variable, bool &positive) const;

    /**
     * @brief Change the weights vector using random numbers in the range (0,1).
     */
    void ChangeWeights();

    /**
     * @brief Function that performs Unit Propagation. If a variable is a unit variable, the assignment of that variable
     * is defined by the value of that variable (if the unit variable appears as positive, the assignment will be true
     * and if the variable appears as negative the assignment will be false).
     */
    void UnitPropagation();

    /**
     * @brief Function that performs a partial assignment. If a variable is true, we have to remove the clauses where
     * that variable appears as positive (because that clause will be satisfied) and remove that variable from the
     * clause where the variable appears as negative.
     * @param variable_index: Index of the variable that is going to be checked.
     * @param assignation: True or false assignation to the variable_index.
     */
    void PartialAssignment(unsigned int variable_index, bool assignation);

    /**
     * @brief Return a complete clause.
     * @param search_clause: Clause to search.
     * @return Vector with all the variable that appears in search_clause. If the variable is true it will appear
     * as positive and it will appear as negative if the variable is false. If the output of this function
     * is discarded, the compiler will raise a warning.
     */
    [[nodiscard]] clause Clause(unsigned int search_clause) const;

    /**
     * @brief Get the clauses where a variable appears.
     * @param variable: Variable to look for.
     * @return A vector with the clauses where variable appears. If the output of this function is discarded,
     * the compiler will raise a warning.
     */
    [[nodiscard]] uvector getClausesOfVariable(int variable) const;

    /**
     * @brief Function that checks if an assignment satisfies or not a given clause.
     * @param assignment: Boolean vector with the assignment (true if positive false if negative).
     * @param search_clause: Clause that will be checked if the assignment satisfies it.
     * @return true if the clause is satisfied and false if not. If the output of this function is discarded,
     * the compiler will raise a warning.
     */
    [[nodiscard]] static bool SatisfiesC(const vector<bool> &assignment, const clause &search_clause) ;

    /**
     * @brief Function that checks if an assignment satisfies or not the formula.
     * @param assign: Boolean vector with the assignment (true if positive false if negative).
     * @param sat: Boolean vector where the ith position will be true if the ith clause is satisfied with the given
     * assignment and false if it is not satisfied.
     * @param indexes: Indexes of the clauses that will be checked.
     * @return true if the formula is satisfied and false if not.
     */
    bool SatisfiesF(const vector<bool> &assign, vector<bool> &sat, const vector<unsigned int> &indexes) const;

    /**
     * @brief Function that gets the break count. From a set of satisfied clauses and given a clause C,
     * this funtion will flip each variable in C and it will count the number of clauses that are still satisfied.
     * @param sat_clauses: Boolean vector where the ith position will be true if the ith clause is satisfied with the
     * given assign. False if it is not satisfied.
     * @param s_clause: Clause that will be used to calculate the break count.
     * @param assign: True assigment that will be used.
     * @param min_index: Index of the variable with the lower break count.
     * @return A vector (same size than s_clause) with the count of clauses that are still satisfied if we flip each
     * variable of the clause. If the output of this function is discarded, the compiler will raise a warning.
     */
    [[nodiscard]] uvector getBreakCount(const vector<bool> &sat_clauses, const clause &s_clause,
                                        const vector<bool> &assign, unsigned int &min_index) const;

    /**
     * @brief WalkSAT algorithm.
     * @param max_tries: Maximum number of tries that the algorithm will do.
     * @param max_flips: Maximum number of flips that the algorithm will do.
     * @param noise: Noise parameter (this will choose if taking a random variable of c or the variable with the lower
     * break count.
     * @param fixed_variables: Vector that will be used to pre-assign variables. If the ith position is 1 the ith
     * variable will be true, -1 will be false and if 0, walksat will be able to change it's assignment.
     * @return A boolean vector with the assignment (if found) that satisfies the formula. If the algorithm hasn't found
     * an assignment, it will return an empty vector. If the output of this function is discarded,
     * the compiler will raise a warning.
     */
    [[nodiscard]] vector<bool>
    WalkSAT(unsigned int max_tries, unsigned int max_flips, double noise, const vector<int>& fixed_variables) const;

    /**
     * @brief Check if an assignment satisfies the formula.
     * @param assignment: Boolean vector with the assignment where the ith position will be the assignment of the
     * ith variable.
     * @return True if the assignment satisfies the entire clause and false if it not.
     */
    [[nodiscard]] bool CheckAssignment(const vector<bool> &assignment) const;

    /**
     * @brief Operator << overload. The output will have the DIMACS syntax.
     * @param out: Ostream object (can be a file, standard output).
     * @param graph: Factor graph object that will be printed out.
     * @return Ostream object with the contents of graph.
     */
    friend std::ostream &operator << (std::ostream &out, const FactorGraph &graph);
};

#endif //FACTOR_GRAPH_H
