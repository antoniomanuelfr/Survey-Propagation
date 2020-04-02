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
 * The variables in a DIMACS file are in the range [1,NumberVariables]
 *  and the 0 variable means that the clause has finished. The representation that is used goes from
 *  [0,NumberVariables). So when we are saying that PositiveClauses[0] we are getting the clauses where the
 *  variable 1 is positive.
 **/
class FactorGraph{
private:
    std::vector<std::vector<int>> PositiveVariables; /**< Vector that storage the positive variables of each clause.  */
    std::vector<std::vector<int>> NegativeVariables; /**< Vector that storage the negative variables of each clause.  */
    std::vector<std::vector<int>> PositiveClauses; /**< Vector that storage for each variable the clauses where
    *  that variable appear as positive.                                                                              */
    std::vector<std::vector<int>> NegativeClauses; /**< Vector that storage for each variable the clauses where
    *  that variable appear as negative.                                                                              */
    std::vector<std::vector<double>> EdgeWeights; /**< Vector that storage the edge pis (weight).                     */
    int NumberClauses; /**< Variable that storage the number of clauses.                                              */
    int NumberVariables; /**< Variable that storage the number of variables.                                          */

public:
    /**
     * @brief Constructor for the FactorGraph class. The NumberClauses and NumberVariables is computed taking
     * the size of the vectors.
     * @param positiveVariables Vector that will be copied in to the PositiveVariables of the object.
     * @param negativeVariables Vector that will be copied in to the NegativeVariables of the object.
     * @param positiveClauses Vector that will be copied in to the PositiveClauses of the object.
     * @param negativeClauses Vector that will be copied in to the NegativeClauses of the object.
     * @param edgeWeights Vector that will be copied in to the EdgeWeights of the object
     */
    FactorGraph(const std::vector<std::vector<int>> &positiveVariables,
                const std::vector<std::vector<int>> &negativeVariables,
                const std::vector<std::vector<int>> &positiveClauses,
                const std::vector<std::vector<int>> &negativeClauses,
                const std::vector<std::vector<double>> &edgeWeights);

    /**
     * @brief Constructor for FactorGraph.
     * @param path: DIMACS file path
     * @param seed: Seed for the random number generator (used for the function LoadEdgeWeights)
     */
    FactorGraph(const std::string &path, int seed = 0);

    /**
     * @brief Getter for EdgeWeights vector.
     * @return A constant reference to EdgePis.
     */
    const std::vector<std::vector<double>> &getEdgeWeights() const{
        return this->EdgeWeights;
    }

    /**
     * @brief Getter for PositiveVariables
     * @return A constant reference to PositiveVariables
     */
    const std::vector<std::vector<int>> &getPositiveVariables() const {
        return PositiveVariables;
    }

    /**
     * @brief Getter por NegativeVariables
     * @return A constant reference to NegativeVariables
     */
    const std::vector<std::vector<int>> &getNegativeVariables() const {
        return NegativeVariables;
    }
    /**
     * @brief Getter por PositiveClauses
     * @return A constant reference to PositiveClauses
     */
    const std::vector<std::vector<int>> &getPositiveClauses() const {
        return PositiveClauses;
    }
    /**
     * @brief Getter por NegativeClauses
     * @return A constant reference to NegativeClauses
     */
    const std::vector<std::vector<int>> &getNVariablesInClauses() const {
        return NegativeClauses;
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
     * @brief Look for the positives and negatives variables in a clause.
     * @param clause: Clause to look.
     * @param positives: Vector where the positive variables that appear in the clause will be storaged.
     * @param negatives: Vector where the negative variables that appear in the clause will be storaged.
     */
    void VariablesInClause(int clause, std::vector<int>& positives, std::vector<int>& negatives) const;

    /**
     * @brief Check if a variable appears in a clause.
     * @param clause: Clause where to look for the variable.
     * @param variable: Variable that will be looked in Clause. If we want to find the variable 5, we have to put
     * 5. (see class description).
     * @param positive: Will be true if the variable was founded in positives nodes, false in other case.
     * @return Index of the variable in the clause. If not found the return value is -1.
     */
    int Connection(unsigned int clause, unsigned int variable, bool &positive) const;

    /**
     * @brief Change the weights vector. If the weights are generated randomly this will be generated following a
     * normal distribution. To change the weights specify a different seed.
     * @param rand: If true random weights will be generated. If it's not true all weights will be 1. Defaults to true.
     * @param seed: Random seed for the number generator. Defaults to 0.
     */
    void LoadEdgeWeights(bool rand = true, unsigned long seed = 0);

    FactorGraph PartialAssignment(const std::vector<bool> &assignment) const;

    /**
     * @brief Operator << overload.
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
