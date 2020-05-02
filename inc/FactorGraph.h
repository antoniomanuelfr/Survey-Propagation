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

/** Create definition for uvector. */
typedef std::vector<unsigned int> uvector;
/** Create definition for clause. */
typedef std::vector<int> clause;
/** Create definition for to umatrix. */
typedef std::vector<std::vector<unsigned int>> umatrix;
/** Create definition for wmatrix. */
typedef std::vector<std::vector<std::pair<int, double>>> wmatrix;

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
     * @brief Read a DIMACS file (the clauses of the DIMACS file must be in conjutctive normal form).
     * and overwrite the Positive and Negative vectors by it's content.
     * @param path: Path to the file.
     * @param n_clauses: int where the number of clauses that was founded will be storaged.
     * @param n_variables: int where the number of variables that was founded will be storaged.
     */
    void ReadDIMACS(const std::string &path, int &n_clauses, int &n_variables);

    /**
     * @brief Function that loads new clauses vectors.
     * @param deleted
     * @param satisfied
     */
    void ApplyNewClauses(const std::vector<std::vector<int>> &deleted, const std::vector<bool> &satisfied);

public:
    FactorGraph() = default;

    /**
     * @brief Copy constructor for FactorGraph class.
     * @param fc Factor graph to copy.
     */
    FactorGraph(const FactorGraph &fc);

    [[maybe_unused]] [[maybe_unused]] /**
     * @brief Constructor for FactorGraph.
     * @param path: DIMACS file path.
     * @param seed: Seed for the random number generator (used for the function LoadEdgeWeights).
     */
    FactorGraph(const std::string &path, int seed = 0);

    /**
     * @brief Getter por PositiveVariables.
     * @return A constant reference to PositiveVariables.
     */
    umatrix getPositiveClauses() const {
        return this->PositiveVariables;
    }

    /**
     * @brief Getter por NegativeVariables.
     * @return A constant reference to NegativeVariables.
     */
    umatrix getNegativeClauses() const {
        return this->NegativeVariables;
    }

    /**
     * @brief Getter for NumberClauses.
     * @return Integer with the value of NumberClauses.
     */
    int getNClauses() const {
        return NumberClauses;
    }

    /**
     * @brief Getter for NumberVariables.
     * @return Integer with the value of NumberVariables.
     */
    int getNVariables() const {
        return NumberVariables;
    }

    /**
     * @brief Getter for a specific edge weight.
     * @param search_clause Clause of the edge.
     * @param variable Variable of the edge.
     * @return The assigned weight of the edge between search_clause and variable.
     */
    double GetEdgeW(unsigned int search_clause, unsigned int variable) const;

    /**
     * @brief Change the weight of a specific edge.
     * @param search_clause Clause of the edge.
     * @param variable Variable of the edge.
     * @param value New weight of the edge.
     */
    void SetEdgeW(unsigned int search_clause, unsigned int variable, double value);

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
    void LoadEdgeWeights(bool rand = true, unsigned long seed = 0);

    /**
     * @brief Function that performs a partial assignment. If a variable is true, we have to remove the clauses where
     * that variable appears as positive (because that clause will be satisfied) and remove that variable from the
     * clause where the variable appears as negative.
     * @param assignment. int vector. If the i position is 1, it represent that the i variable will be true.
     * If the position is 0, it will be ignored and if it is -1, it will be consider as false.
     * @return A FactorGraph with the partial assignment applied.
     */
    FactorGraph PartialAssignment(const std::vector<int> &assignment);

    /**
     * @brief Return the complete search_clause.
     * @param search_clause Clause to search.
     * @param ignore Ignore certain variable. Defaults to 0.
     * @return Vector that contains the search_clause.
     */
    clause Clause(unsigned int search_clause, unsigned int ignore = 0) const;

    /**
     * @brief Get the clauses where a variable appears.
     * @param variable Variable to look for
     * @return A vector with the clauses where variable appears.
     */
    uvector ClausesOfVariable(unsigned int variable) const;

    /**
     * @brief Look for the positives and negatives variables in a clause.
     * @param clause: Clause to look.
     * @param positives: Vector where the positive variables that appear in the clause will be storaged.
     * @param negatives: Vector where the negative variables that appear in the clause will be storaged.
     */
    void VariablesInClause(unsigned int clause, uvector &positives, uvector &negatives) const;

    /**
     * @brief Operator << overload. The output will have the DIMACS syntax.
     * @param out: Ostream object (can be a file, standard output).
     * @param graph: Factor graph object that will be printed to out.
     * @return Ostream object with the contents of graph.
     */
    friend std::ostream &operator<<(std::ostream &out, const FactorGraph &graph);
};

/**
 * @brief Split a string separated by a delimiter.
 * @param str: String that is going to be splitted.
 * @param delim: Delimiter. By default is an space
 * @return A vector<string> with the splits of the string.
 */
std::vector<std::string> SplitString(const std::string& str, char delim = ' ');
#endif //FACTOR_GRAPH_H
