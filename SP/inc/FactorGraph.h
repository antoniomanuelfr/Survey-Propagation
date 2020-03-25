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
/*
 * @brief:
 *
 *
 **/
class FactorGraph{
private:
    std::vector<std::vector<int>> NegativeNodes;  /** < Vector that storages the postive literals.  */
    std::vector<std::vector<int>> PositiveNodes;  /** < Vector that storages the negative literals. */
    std::vector<std::vector<float>> EdgePis;      /** < Vector that storages the edge pis (weight). */
public:
    FactorGraph(const std::string& path);

    const std::vector<std::vector<int>> &getNegativeNodes() const;
    const std::vector<std::vector<int>> &getPositiveNodes() const;
    const std::vector<std::vector<float>> &getEdgePis() const;

};
#endif //FACTOR_GRAPH_H
