//
// Created by antoniomanuelfr on 4/10/20.
//

#ifndef SURVEY_PROPAGATION_H
#define SURVEY_PROPAGATION_H

#include <utility>

#include "FactorGraph.h"
/**
 * @brief Class for the implementation of the survey propagation algorithm.
 */
class SurveyPropagation {
private:
    /** Factor Graph of the formula that is going to be checked. */
    FactorGraph AssociatedGraph;
    /** Number of iterations of the algorithm. */
    unsigned int n_iters;
    /** Precision value. */
    double precision;
    /** Random seed that is going to be used */
    int seed;
    /**
     * @brief Function that implements the SP-Update function.
     * @param search_clause Clause that is going going to be searched.
     * @param variable Variable that is going to be searched.
     */
    void Update(unsigned int search_clause, unsigned int variable);

    bool SP ();
public:
    /**
     * @brief Constructor for the Survey Propagation class.
     * @param AssociatedGraph FactorGraph object with the formula that is going to be used.
     * @param n_iters Maximum number of iterations. Defaults to 1000.
     * @param precision Precision of the algorithm. Defaults to 0.1.
     */
    explicit SurveyPropagation(const FactorGraph &AssociatedGraph, unsigned int n_iters = 100,
            double precision = 0.1, int seed = 0) {
        this->AssociatedGraph = AssociatedGraph;
        this->n_iters = n_iters;
        this->precision = precision;
        this->seed = seed;
    }
    int SID();
};


#endif // SURVEY_PROPAGATION_H
