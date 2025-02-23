//
// Created by antoniomanuelfr on 4/10/20.
//

#ifndef SURVEY_PROPAGATION_H
#define SURVEY_PROPAGATION_H

#define SP_UNCONVERGED -1
#define SP_CONVERGED 1
#define SAT 1
#define PROB_UNSAT 0
#define CONTRADICTION -2

#include <utility>
#include "FactorGraph.h"

/**
 * @brief Class for the implementation of the survey propagation algorithm.
 */
class SurveyPropagation {

private:

    /** Factor Graph of the formula that is going to be checked. */
    FactorGraph *AssociatedGraph;
    /** Number of iterations of the algorithm. */
    unsigned int n_iters;
    /** Precision value. */
    double precision;
    /** If a survey is lower than lower_bound, the survey will be 0. */
    double lower_bound;
    /** Number of iterations for WalkSAT algorithm. */
    unsigned int walksat_iters;
    /** Number of flips for WalkSAT algorithm */
    unsigned int walksat_flips;
    /** Noise for WalkSAT algorithm. */
    double walksat_noise;
    /** Seed for the RNG. */
    int seed;

    /**
     * @brief Function that implements the SP-Update function.
     * @param search_clause: Clause that is going going to be searched.
     * @param variable: Variable that is going to be searched.
     */
    void Update(unsigned int search_clause, int variable);

    /**
     * @brief Function that implements the SP function.
     * @param trivial: Will be true if the surveys are trivial (all surveys equal to zero).
     * @return It will return SP_UNCONVERGED if SP hasn't converged or SP_CONVERGED if SP has converged.
     */
    [[nodiscard]] int SP(bool &trivial);

    /**
     * @brief Function that calculate the biases once all surveys have been updated.
     * @param positive_w: Vector where the positive biases of each variable will be stored.
     * @param negative_w: Vector where the negative biases of each variable will be stored.
     * @param zero_w: Vector where the zero biases of each variable will be stored.
     * @param max_index: Index of the variable that with the largest difference between the positive and negative bias.
     */
    void CalculateBiases(vector<double> &positive_w, vector<double> &negative_w, vector<double> &zero_w, int &max_index);

public:

    /**
     * @brief Constructor for the Survey Propagation class.
     * @param path FactorGraph object with the formula that is going to be used.
     * @param seed: Seed for the RNG. Defaults to 1.
     * @param n_iters: Maximum number of iterations. Defaults to 1000.
     * @param precision: Precision of the algorithm. Defaults to 0.1.
     * @param bound: If a survey is lower than bound, it will be set to 0.
     * @param w_iters: Number of iteration for WalkSAT algorithm. Defaults to 1000.
     * @param flips: Number of flips for WalkSAT algorithm. Defaults to 100.
     * @param noise: Noise parameter for WalkSAT algorithm. Defaults to 0.57.
     */
    explicit SurveyPropagation(const std::string& path, int seed = 1, unsigned int n_iters = 10e3,
                               double precision = 10e-3,
                               double bound = 1e-16, unsigned int w_iters = 1000, unsigned int flips = 100,
                               double noise = 0.57) {
        this->seed = seed;
        this->AssociatedGraph = new FactorGraph(path, this->seed);
        this->n_iters = n_iters;
        this->precision = precision;
        this->lower_bound = bound;
        this->walksat_iters = w_iters;
        this->walksat_flips = flips;
        this->walksat_noise = noise;
    }

    /**
     * @brief Destructor for the SurveyPropagation class.
     */
    ~SurveyPropagation() {
        delete this->AssociatedGraph;
    }

    /**
     * @brief Function that implements the SID (Survey Inspired Decimation) function.
     * @param true_assignment: Boolean vector with the true assignment finded by the SID process.
     * @param sid_iters: Number of iterations of the SID process.
     * @return SP_UNCONVERGED, PROB_UNSAT, SAT.
     */
    [[nodiscard]] int SID(vector<bool> &true_assignment, unsigned int sid_iters);

    /**
     * @brief Function that implements the SID (Survey Inspired Decimation) function using a number of fixed variables (
     * f times the number of variables).
     * @param true_assignment: Boolean vector with the true assignment finded by the SID process.
     * @param f: Fractions of variables that will be fixed.
     * @return SP_UNCONVERGED, PROB_UNSAT, SAT.
     */
    [[nodiscard]] int SIDF(vector<bool> &true_assignment, double f);


};
#endif // SURVEY_PROPAGATION_H
