//
// Created by antoniomanuelfr on 4/10/20.
//

#ifndef SURVEY_PROPAGATION_H
#define SURVEY_PROPAGATION_H

#define SP_UNCONVERGED -1
#define SP_CONVERGED 1
#define SAT 1
#define PROB_UNSAT 0
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
    /** Random seed that is going to be used. */
    int seed;
    /** If a survey is lower than lower_bound, the survey will be 0. */
    double lower_bound;
    /** Number of iterations for WalkSAT algorithm. */
    unsigned int walksat_iters;
    /** Number of flips for WalkSAT algorithm */
    unsigned int walksat_flips;
    /** Noise for WalkSAT algorithm. */
    double walksat_noise;
    /**
     * @brief Function that implements the SP-Update function.
     * @param search_clause Clause that is going going to be searched.
     * @param variable Variable that is going to be searched.
     */
    void Update(unsigned int search_clause, unsigned int variable);

    /**
     * @brief Function that implements the SP function
     * @param trivial
     * @return
     */
    int SP(bool &trivial);

    void CalculateBiases(std::vector<double> &positive_w, std::vector<double> &negative_w,
                         std::vector<double> &zero_w, int &max_index);
public:
    /**
     * @brief Constructor for the Survey Propagation class.
     * @param AssociatedGraph FactorGraph object with the formula that is going to be used.
     * @param n_iters Maximum number of iterations. Defaults to 1000.
     * @param precision Precision of the algorithm. Defaults to 0.1.
     */
    explicit SurveyPropagation(const FactorGraph &AssociatedGraph, unsigned int n_iters = 100, double precision = 0.1,
            int seed = 0, double bound = 1e-6, unsigned int w_iters = 1000, unsigned flips = 100, double noise = 0.5) {
        this->AssociatedGraph = AssociatedGraph;
        this->n_iters = n_iters;
        this->precision = precision;
        this->seed = seed;
        this->lower_bound = bound;
        this->walksat_iters = w_iters;
        this->walksat_flips = flips;
        this->walksat_noise = noise;
    }
    int SID(std::vector<bool> &true_assignment);
};


#endif // SURVEY_PROPAGATION_H
