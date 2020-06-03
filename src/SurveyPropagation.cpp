//
// Created by antoniomanuelfr on 4/10/20.
//

#include <z3.h>
#include "SurveyPropagation.h"

void SurveyPropagation::Update(unsigned int search_clause, int variable) {
    // Preconditions: Clause and variable must be in the range and there has to be a connection.
    if (search_clause > this->AssociatedGraph.getNClauses() || variable > this->AssociatedGraph.getNVariables()) {
        return;
    }

    clause va;
    int index = -1;
    unsigned int aux_index;
    uvector va_u, va_s;
    double survey = 1.0, weight;
    double product_u, product_s, pi_u, pi_s, pi_0;

    // Get V(search_clause)
    va = this->AssociatedGraph.Clause(search_clause);
    // For every variable j of va (except for the variable)
    for (int j = 0; j < va.size(); j++) {
        if (va[j] != variable) {
            product_s = product_u = pi_0 = 1.0;

            // Get the va_s (clauses where j appears with the same sign) and
            // v_u (clauses where j appears with the opposite sign) sets.
            if (va[j] > 0) {
                va_s = this->AssociatedGraph.getPositiveClausesOfVariable(va[j]);
                va_u = this->AssociatedGraph.getNegativeClausesOfVariable(va[j]);
            }
            else {
                va_s = this->AssociatedGraph.getNegativeClausesOfVariable(va[j]);
                va_u = this->AssociatedGraph.getPositiveClausesOfVariable(va[j]);
            }

            // Calculation of product u
            for (int b : va_u) {
                if (b != search_clause) {
                    aux_index = this->AssociatedGraph.getIndexOfVariable(b, va[j]);
                    weight = 1.0 - this->AssociatedGraph.getEdgeW(b, aux_index);
                    product_u *= weight;
                    pi_0 *= weight;
                }
            }

            // Calculation of product s
            for (int b : va_s) {
                if (b != search_clause) {
                    aux_index = this->AssociatedGraph.getIndexOfVariable(b, va[j]);
                    weight = 1.0 - this->AssociatedGraph.getEdgeW(b, aux_index);
                    product_s *= weight;
                    pi_0 *= weight;
                }
            }

            product_u = product_u < this->lower_bound ? 0.0 : product_u;
            product_s = product_s < this->lower_bound ? 0.0 : product_s;
            pi_0 = pi_0 < this->lower_bound ? 0.0 : pi_0;
            pi_u = (1.0 - product_u) * product_s;
            pi_s = (1.0 - product_s) * product_u;

            survey *= pi_u / (pi_u + pi_s + pi_0);
        } else {
            // Get the index for the setEdgeW function.
            index = j;
        }
    }
    // All the pis are calculated, so we calculate the survey
    this->AssociatedGraph.setEdgeW(search_clause, index, survey < this->lower_bound ? 0.0 : survey);
}

int SurveyPropagation::SP(bool &trivial) {
    bool converged;
    wmatrix prev_warnings;
    std::default_random_engine generator(seed); // Random engine generator.
    std::uniform_real_distribution<double> distribution(0, 1); //Distribution for the random generator.
    uvector clauses_indexes(this->AssociatedGraph.getNClauses()), var_indexes;
    clause clause;
    std::iota(clauses_indexes.begin(), clauses_indexes.end(), 0); // Generate the indexes vector.
    for (int iters = 0; iters < this->n_iters; iters++) {
        converged = true;
        trivial = true;
        prev_warnings = this->AssociatedGraph.getMatrix();
        // Choose random clauses without repetition.
        std::shuffle(clauses_indexes.begin(), clauses_indexes.end(), generator);
        for (int index : clauses_indexes) {
            var_indexes.clear();
            clause = this->AssociatedGraph.Clause(index);
            // Choose random variable from the clause without repetition.
            var_indexes.resize(clause.size());
            std::iota(var_indexes.begin(), var_indexes.end(), 0);
            std::shuffle(var_indexes.begin(), var_indexes.end(), generator);
            // Update every edge.
            for (int i : var_indexes) {
                this->Update(index, clause[i]);
            }
        }
        // Check the convergence condition.
        for (int i = 0; i < prev_warnings.size() && converged; i++) {
            for (int j = 0; j < prev_warnings[i].size(); j++) {
                trivial = trivial ? this->AssociatedGraph.getEdgeW(i, j) == 0 : trivial;
                if (std::abs(this->AssociatedGraph.getEdgeW(i, j) - prev_warnings[i][j]) > this->precision) {
                    converged = false;
                    break;
                }
            }
        }
        // If the variable is true, means that all the the edge
        if (converged) {
            std::cout << "Survey propagation has converged.";
            return SP_CONVERGED;
        }
    }
    std::cout << "Survey propagation hasn't converged." << std::endl;
    return SP_UNCONVERGED;
}

void SurveyPropagation::CalculateBiases(vector<double> &positive_w, vector<double> &negative_w, vector<double> &zero_w,
                                        int &max_index) {
    if (!positive_w.empty()) {
        positive_w.clear();
    }

    if (!negative_w.empty()) {
        negative_w.clear();
    }

    if (!zero_w.empty()) {
        zero_w.clear();
    }
    positive_w.resize(this->AssociatedGraph.getNVariables());
    negative_w.resize(this->AssociatedGraph.getNVariables());
    zero_w.resize(this->AssociatedGraph.getNVariables());
    max_index = 0;

    uvector positive_clauses, negative_clauses;
    clause actual_clause;
    unsigned int var_index_clause, variable_index;
    double positive_pi, negative_pi, zero_pi, pos_prod, neg_prod, survey, max = 0.0;

    // For each variable we have to calculate the three pis.
    for (int variable = 1; variable <= this->AssociatedGraph.getNVariables(); variable++) {
        variable_index = variable - 1;
        pos_prod = 1.0;
        neg_prod = 1.0;
        zero_pi = 1.0;
        // Get the V_+ and V_- sets (clauses where the variable appears as positive and negative
        positive_clauses = this->AssociatedGraph.getPositiveClausesOfVariable(variable);
        negative_clauses = this->AssociatedGraph.getNegativeClausesOfVariable(variable);
        // Positive PI of variable
        for (auto it : positive_clauses) {
            actual_clause = this->AssociatedGraph.Clause(it);
            var_index_clause = std::distance(actual_clause.begin(), std::find(actual_clause.begin(), actual_clause.end(), variable));
            survey = this->AssociatedGraph.getEdgeW(it, var_index_clause);
            pos_prod *= (1.0 - survey);
            zero_pi *= (1.0 - survey);
        }
        // Positive PI of variable
        for (auto it : negative_clauses) {
            actual_clause = this->AssociatedGraph.Clause(it);
            int index = std::distance(actual_clause.begin(),std::find(actual_clause.begin(), actual_clause.end(), -(variable)));
            survey = this->AssociatedGraph.getEdgeW(it, index);
            neg_prod *= (1.0 - survey);
            zero_pi *= (1.0 - survey);
        }
        positive_pi = (1.0 - pos_prod) * neg_prod;
        negative_pi = (1.0 - neg_prod) * pos_prod;

        positive_w[variable_index] = positive_pi / (positive_pi + negative_pi + zero_pi);
        negative_w[variable_index] = negative_pi / (positive_pi + negative_pi + zero_pi);
        zero_w[variable_index] = 1.0 - positive_w[variable_index] - negative_w[variable_index];
        double difference = std::abs(positive_w[variable_index] - negative_w[variable_index]);
        if (difference > max) {
            max = difference;
            max_index = variable;
        }
    }
}

int SurveyPropagation::SID(vector<bool> &true_assignment, unsigned int sid_iters) {
    if (!true_assignment.empty()) {
        true_assignment.clear();
    }
    true_assignment.resize(this->AssociatedGraph.getNVariables(), false);
    bool trivial_surveys;
    int max_index;
    vector<double> positive_w, negative_w, zero_w;
    vector<bool> walksat_assignment;
    for (int iter = 0; iter < sid_iters; iter++) {
        // The surveys are randomized by default.
        if (this->SP(trivial_surveys) == SP_CONVERGED) {
            // Decimate process, check if the surveys aren't trivial.
            if (!trivial_surveys) {
                std::cout << "The surveys are not trivial, starting decimate process." << std::endl;
                this->CalculateBiases(positive_w, negative_w, zero_w, max_index);
                this->AssociatedGraph.PartialAssignment(max_index, positive_w[max_index] > negative_w[max_index]);
            } else {
                std::cout << "The surveys are trivial, starting local search." << std::endl;
                walksat_assignment = this->AssociatedGraph.WalkSAT(this->walksat_iters, this->walksat_flips,
                                                                   this->walksat_noise);
                // If WalkSAT has found an assignment, return the assignment.
                true_assignment = walksat_assignment;
                return true_assignment.empty() ? PROB_UNSAT : SAT;
                }
            // Calling unit propagation with the assignment applied.
            AssociatedGraph.UnitPropagation(true_assignment);
            // If there is a contradiction, we return CONTRADICTION
            if (AssociatedGraph.Contradiction()) {
                true_assignment.clear();
                return CONTRADICTION;

            } else if (AssociatedGraph.EmptyClause()) {  // If the graph is the empty clause we return SAT.
                return SAT;
            }
        } else {
            // If SP has not converged, return SP_UNCONVERGED
            true_assignment.clear();
            return SP_UNCONVERGED;
        }
    }
    true_assignment.clear();
    return PROB_UNSAT;
}


