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
    FactorGraph(const std::string& path, int = 0);
    /**
     * @brief Getter for EdgeWeights vector.
     * @return A constant reference to EdgePis.
     */
    const std::vector<std::vector<double>> &getEdgeWeights() const{
        return this->EdgeWeights;
    }
    /**
     * @brief Read a DIMACS file (the clauses of the DIMACS file must be in conjutctive normal form)
     * and overwrite the Positive and Negative vectors by it's content.
     * @param String with the path to the file.
     */
    void ReadDIMACS(const std::string&, int&, int&);
    /**
     * @brief Look for the positives and negatives variables in a clause. This function does NOT initialize the
     *  NumberVariables and NumberClauses variables.
     * @param Clause to look.
     * @param Vector where the positive variables that appear in the clause will be storaged.
     * @param Vector where the negative variables that appear in the clause will be storaged.
     */
    void VariablesInClause(int, std::vector<int>&, std::vector<int>&) const;
    /**
     * @brief Check if a variable appears in a clause.
     * @param Clause where to look.
     * @param Variable to look.
     * @param True if the variable was founded in positives nodes, false in other case.
     * @return Index of the variable in the clause. If not found the return value is -1.
     */
    int Connection(unsigned int, unsigned int, bool&) const ;
    /**
     * @brief Change the weights vector. If the weights are generated randomly this will be generated following a
     * normal distribution.
     * @param If true random weights will be generated. If it's not true all weights will be 1. Defaults to true.
     * @param Random seed for the number generator. Defaults to 0.
     */
    void LoadEdgeWeights(bool = true, unsigned long = 0);

};
/**
 * @brief Split a string separated by a delimiter.
 * @param String that is going to be splitted.
 * @param Delimiter. By default is an space
 * @return A vector<string> with the splits of the string.
 */
std::vector<std::string> SplitString(const std::string&, char  = ' ');
#endif //FACTOR_GRAPH_H
