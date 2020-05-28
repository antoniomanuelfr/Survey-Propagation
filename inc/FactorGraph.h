//
// Created by antoniomanuelfr on 3/25/20.
//

#ifndef FACTOR_GRAPH_H
#define FACTOR_GRAPH_H
#define SAT 1
#define PROB_UNSAT 0
#define UNSAT -1
#define EMPTY -2
#include <vector>
#include <utility>
#include <iostream>
#include <string>
#include <fstream>
#include <algorithm>
#include <random>
#include <unordered_map>

/** Create definition for uvector. Stands for an unsigned vector of the standard library. */
typedef std::vector<unsigned int> uvector;
/** Create definition for clause. Stands for an int vector of the standard library. */
typedef std::vector<int> clause;
/** Create definition for to umatrix. Stands for a unsigned int matrix of standard library. */
typedef std::vector<std::vector<unsigned int>> umatrix;
/** Create definition for wmatrix. Stands for a double matrix of the standard library. */
typedef std::vector<std::vector<double>> wmatrix;

/**
 * @brief Operator == for two clauses.
 * @param c1: First clause.
 * @param c2: Second clause.
 * @return True if the two clauses are equal (element by element).
 */
bool operator == (const clause &c1, const clause &c2);
/**
 * @brief Class for handle the factor graph representation of a CNF formula.
 *  The variables in a DIMACS file are in the range [1,NumberVariables]
 *  and the 0 variable means that the clause has finished. The representation that is used goes from
 *  [0,NumberVariables). So when we are saying that PositiveVariables[0] we are getting the clauses where the
 *  variable 1 is positive.
 **/
class FactorGraph {
private:
    /** Vector that storage the positive variables of each clause. */
    umatrix PositiveClauses;
    /** Vector that storage the negative variables of each clause. */
    umatrix NegativeClauses;
    /** Vector that storage for each variable the clauses where that variable appear as positive. */
    umatrix PositiveVariables;
    /** Vector that storage for each variable the clauses where that variable appear as negative. */
    umatrix NegativeVariables;
    /** Vector that storage the edge pis (weight). */
    wmatrix EdgeWeights;
    /** Variable that storage the number of clauses. */
    int NumberClauses{0};
    /** Variable that storage the number of variables. */
    int NumberVariables{0};

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
    void ApplyNewClauses(const std::vector<std::vector<int>> &deleted, const std::vector<bool> &satisfied);

public:
    /**
     * @brief Empty constructor.
     */
    FactorGraph() = default;
    /**
     * @brief Copy constructor for FactorGraph class.
     * @param fc: Factor graph to copy.
     */
    FactorGraph(const FactorGraph &fc);

    /**
     * @brief Constructor for FactorGraph.
     * @param path: DIMACS file path.
     * @param seed: Seed for the random number generator (used for the function RandomizeWeights).
     */
    explicit FactorGraph(const std::string &path, int seed = 0);

    /**
     * @brief Getter for NumberClauses.
     * @return Integer with the value of NumberClauses.
     */
    [[nodiscard]] int getNClauses() const {
        return NumberClauses;
    }

    /**
     * @brief Getter for NumberVariables.
     * @return Integer with the value of NumberVariables.
     */
    [[nodiscard]] int getNVariables() const {
        return NumberVariables;
    }

    /**
     * @brief Check if the formula is an empty clause.
     * @return True if the formula is an empty clause or false if not
     */
    [[nodiscard]] bool EmptyClause() const {
        return (this->NumberClauses == 0) ;
    }

    /**
    * @brief Check if the formula is a contradiction.
    * @return True if the formula is a contradiction or false if not
    */
    [[nodiscard]] bool Contradiction() const {
        return (this->NumberClauses == 1) && (this->Clause(0).empty());
    }

    /**
     * @brief Get all the edges of a clause.
     * @param search_clause: Clause that will be looked.
     * @return Vector with all the edges of search_clause.
     */
    std::vector<double> getEdge(unsigned int search_clause) {
        return this->EdgeWeights[search_clause];
    }

    /**
     * @brief Getter for a specific edge weight.
     * @param search_clause: Clause of the edge.
     * @param variable: Variable of the edge.
     * @return The assigned weight of the edge between search_clause and variable.
     */
    [[nodiscard]] double getEdgeW(unsigned int search_clause, unsigned int variable) const;

    /**
     * @brief Function that return the weights matrix.
     * @return An wmatrix object.
     */
    [[nodiscard]] wmatrix getMatrix() const {
        return this->EdgeWeights;
    }

    /**
     * @brief Function that returns the unit vars.
     * @return A vector with the unit variables of the formula.
     */
    [[nodiscard]] uvector getUnitVars() const;

    /**
     * @brief Change the weight of a specific edge.
     * @param search_clause: Clause of the edge.
     * @param variable: Variable of the edge.
     * @param value: New weight of the edge.
     */
    void setEdgeW(unsigned int search_clause, unsigned int variable, double value);

    /**
     * @brief Check if a variable appears in a search_clause.
     * @param search_clause: Clause where to look for the variable.
     * @param variable: Variable that will be looked in Clause. If we want to find the variable 5, we have to put
     * 5. (see class description).
     * @param positive: Will be true if the variable was founded in positives nodes, false in other case.
     * @return Index of the variable in the search_clause. If not found the return value is -1.
     */
    int Connection(unsigned int search_clause, unsigned int variable, bool &positive) const;

    /**
     * @brief Change the weights vector. If the weights are generated randomly this will be generated following a
     * normal distribution. To change the weights specify a different seed.
     * @param rand: If true random weights will be generated. If it's not true all weights will be 1. Defaults to true.
     * @param seed: Random seed for the number generator. Defaults to 0.
     */
    void RandomizeWeights(bool rand = true, unsigned long seed = 0);

    /**
     * @brief Function that performs a partial assignment. If a variable is true, we have to remove the clauses where
     * that variable appears as positive (because that clause will be satisfied) and remove that variable from the
     * clause where the variable appears as negative. This function is used in the SP function.
     * @param variable: Index of the variable that is going to be checked.
     * @param assignation: True or false assignation to the variable.
     * @param unit_vars: Vector where the founded unit vars will be stored.
     * @return A FactorGraph with the partial assignment applied.
     */
    FactorGraph PartialAssignmentUP(unsigned int variable, bool assignation, std::vector<unsigned int> &unit_vars);

    /**
     * @brief Function that performs a partial assignment. If a variable is true, we have to remove the clauses where
     * that variable appears as positive (because that clause will be satisfied) and remove that variable from the
     * clause where the variable appears as negative.
     * @param variable: Index of the variable that is going to be checked.
     * @param assignation: True or false assignation to the variable.
     * @return A FactorGraph with the partial assignment applied.
    */
    FactorGraph PartialAssignment(unsigned int variable, bool assignation);

    /**
     * @brief Return the complete search_clause.
     * @param search_clause: Clause to search.
     * @return Vector that contains the search_clause.
     */
    [[nodiscard]] clause Clause(unsigned int search_clause) const;

    /**
     * @brief Get the clauses where a variable appears.
     * @param variable: Variable to look for.
     * @return A vector with the clauses where variable appears.
     */
    [[nodiscard]] uvector ClausesOfVariable(unsigned int variable) const;

    /**
     * @brief Look for the positives and negatives variables in a clause.
     * @param clause: Clause to look.
     * @param positives: Vector where the positive variables that appear in the clause will be stored.
     * @param negatives: Vector where the negative variables that appear in the clause will be stored.
     */
    void VariablesInClause(unsigned int clause, uvector &positives, uvector &negatives) const;

    /**
     * @brief Look for the positives and negatives clauses of a variable_index.
     * @param variable_index: variable_index to look.
     * @param positives: Vector where the clauses in which the variable_index appear as positive will be stored.
     * @param negatives: Vector where the clauses in which the variable_index appear as negative will be stored.
     */
    void ClausesInVariable(unsigned int variable_index, uvector &positive, uvector &negatives) const;

    /**
     * @brief Function that checks if an assignment satisfies or not a given clause.
     * @param assignment: Boolean vector with the assignment (true if positive false if negative).
     * @param search_clause: Clause that will be checked if the assignment satisfies it.
     * @return true if the clause is satisfied and false if not.
     */
    [[nodiscard]] bool SatisfiesC(const std::vector<bool> &assignment, const clause &search_clause) const;

    /**
     * @brief Function that checks if an assignment satisfies or not the formula.
     * @param assignment: Boolean vector with the assignment (true if positive false if negative).
     * @param not_satisfied_clauses: Vector that will store the not satisfied clauses.
     * @param satisfied_clauses: Vector that will store the satisfied clauses.
     * @return true if the formula is satisfied and false if not.
     */
    bool SatisfiesF(const std::vector<bool> &assignment, std::vector<unsigned int> &not_satisfied_clauses,
                    std::vector<unsigned int> &satisfied_clauses, std::vector<unsigned int> &indexes) const;

    /**
     * @brief Function that gets the break count from a set of satisfied clauses given a clause C (each variable in C is
     * flipped and count the number of clauses that are still satisfied.
     * @param satis_clauses: Clauses that are satisfied with the given assignment.
     * @param s_clause: Clause.
     * @param assignment: True assigment that will be used.
     * @return A vector (same size than s_clause) with the count of clauses that are still satisfied if we flip each
     * variable of the clause.
     */
    [[nodiscard]] std::vector<unsigned int> getBreakCount(const std::vector<unsigned int> &satis_clauses, const clause &s_clause,
                                            const std::vector<bool> &assignment) const;

    /**
     * @brief WalkSAT algorithm for FactorGraph class.
     * @param max_tries: Maximum number of tries that the algorithm will do.
     * @param max_flips: Maximum number of flips that the algorithm will do.
     * @param noise: Noise parameter (this will choose if taking a random variable of c or the variable with the lower
     * break count.
     * @param seed: Seed that will be used for the random number generators. Defaults to 0.
     * @return A boolean vector with the assignment (if found) that satisfies the formula. If the algorithm hasn't found
     * an assignment, it will return an empty vector.
     */
    [[nodiscard]] std::vector<bool> WalkSAT(unsigned int max_tries, unsigned int max_flips, double noise, int seed = 0) const;

    /**
     * @brief Operator << overload. The output will have the DIMACS syntax.
     * @param out: Ostream object (can be a file, standard output).
     * @param graph: Factor graph object that will be printed out.
     * @return Ostream object with the contents of graph.
     */
    friend std::ostream &operator << (std::ostream &out, const FactorGraph &graph);
};

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
std::vector<std::string> SplitString(const std::string& str, char delim = ' ');
#endif //FACTOR_GRAPH_H
