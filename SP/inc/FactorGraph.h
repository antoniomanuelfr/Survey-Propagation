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

/**
 * @brief Class for handle the factor graph representation of a CNF formula.
 **/
class FactorGraph{
private:
    std::vector<std::vector<int>> PositiveNodes;  /**< Vector that storage the negative literals.    */
    std::vector<std::vector<int>> NegativeNodes;  /**< Vector that storage the positive literals.    */
    std::vector<std::vector<double>> EdgeWeights; /**< Vector that storage the edge pis (weight).    */
    int NumberClauses;                            /**< Integer that storage the number of clauses    */
    int NumberVariables;                          /**< Integer that storage the number of variables  */

public:

    /**
     * @brief Constructor for FactorGraph
     * @param path: DIMACS file path
     * @param seed: Seed for the random number generator (used for the function LoadEdgeWeights)
     */
    FactorGraph(const std::string& path, int seed = 0);

    /**
     * @brief Getter for EdgeWeights vector.
     * @return A constant reference to EdgePis.
     */
    const std::vector<std::vector<double>> &getEdgeWeights() const{
        return this->EdgeWeights;
    }

    /**
     * @brief Getter por PositiveNodes
     * @return A constant reference to PositiveNodes
     */
    const std::vector<std::vector<int>> &getPositiveNodes() const {
        return PositiveNodes;
    }

    /**
     * @brief Getter por Negative
     * @return A constant reference to NegativeNodes
     */
    const std::vector<std::vector<int>> &getNegativeNodes() const {
        return NegativeNodes;
    }

    /**
     * @brief Getter for NumberClauses
     * @return Integer with the value of NumberClauses
     */
    int getNumberClauses() const {
        return NumberClauses;
    }

    /**
     * @brief Getter for NumberVariables
     * @return Integer with the value of NumberVariables
     */
    int getNumberVariables() const {
        return NumberVariables;
    }

    /**
     * @brief Read a DIMACS file (the clauses of the DIMACS file must be in conjutctive normal form)
     * and overwrite the Positive and Negative vectors by it's content.
     * @param path: Path to the file.
     * @param n_clauses: int where the number of clauses that was founded will be storaged
     * @param n_variables: int where the number of variables that was founded will be storaged

     */
    void ReadDIMACS(const std::string& path, int& n_clauses, int& n_variables);

    /**
     * @brief Look for the positives and negatives variables in a clause. This function does NOT initialize the
     *  NumberVariables and NumberClauses variables.
     * @param clause: Clause to look.
     * @param positives: Vector where the positive variables that appear in the clause will be storaged.
     * @param negatives: Vector where the negative variables that appear in the clause will be storaged.
     */
    void VariablesInClause(int clause, std::vector<int>& positives, std::vector<int>& negatives) const;

    /**
     * @brief Check if a variable appears in a clause.
     * @param clause: Clause where to look for the variable.
     * @param variable: Variable that will be looked in Clause.
     * @param positive: Will be true if the variable was founded in positives nodes, false in other case.
     * @return Index of the variable in the clause. If not found the return value is -1.
     */
    int Connection(unsigned int clause, unsigned int variable, bool& positive) const ;

    /**
     * @brief Change the weights vector. If the weights are generated randomly this will be generated following a
     * normal distribution. To change the weights specify a different seed.
     * @param rand: If true random weights will be generated. If it's not true all weights will be 1. Defaults to true.
     * @param seed: Random seed for the number generator. Defaults to 0.
     */
    void LoadEdgeWeights(bool rand = true, unsigned long seed = 0);
};

/**
 * @brief Split a string separated by a delimiter.
 * @param str: String that is going to be splitted.
 * @param delim: Delimiter. By default is an space
 * @return A vector<string> with the splits of the string.
 */
std::vector<std::string> SplitString(const std::string& str, char delim = ' ');
#endif //FACTOR_GRAPH_H
