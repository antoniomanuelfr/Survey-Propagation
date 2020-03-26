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
#include <iterator>
#include <sstream>

/**
 * @brief Class for handle the factor graph representation of a CNF formula.
 **/
class FactorGraph{
private:
    std::vector<std::vector<int>> PositiveNodes;  /**< Vector that storage the negative literals. */
    std::vector<std::vector<int>> NegativeNodes;  /**< Vector that storage the positive literals.  */
    std::vector<std::vector<float>> EdgePis;      /**< Vector that storage the edge pis (weight). */
public:
    FactorGraph(const std::string& path);
    /**
     * @brief Getter for PositveNodes vector.
     * @return A constant reference to PositveNodes.
     */
    const std::vector<std::vector<int>> &getPositiveNodes() const;
    /**
     * @brief Getter for NegativeNodes vector.
     * @return A constant reference to NegativesNodes.
     */
    const std::vector<std::vector<int>> &getNegativeNodes() const;
    /**
     * @brief Getter for EdgePis vector.
     * @return A constant reference to EdgePis.
     */
    const std::vector<std::vector<float>> &getEdgePis() const;
    /**
     * @brief Function that reads a DIMACS file and overwrite the Positive and Negative vectors by it's content.
     * @param String with the path to the file.
     */
    void ReadDIMACS(const std::string& path);
};
/**
 * @brief Function that splits a string separated by spaces.
 * @param str String that is going to be splitted.
 * @return A vector<string> with the splits of the string.
 */
std::vector<std::string> SplitString(const std::string& str);
#endif //FACTOR_GRAPH_H
