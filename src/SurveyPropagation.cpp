//
// Created by antoniomanuelfr on 4/10/20.
//

#include "SurveyPropagation.h"

void SurveyPropagation::Update(unsigned int search_clause, unsigned int variable) {
    // Preconditions: Clause and variable must be in the range and there has to be a connection.
    if (search_clause > this->AssociatedGraph.getNClauses() || variable > this->AssociatedGraph.getNVariables()) {
        return;
    }

    bool connection_type;
    clause va;
    int index = -1;
    uvector va_u, va_s;
    double survey = 1.0;
    double product_u = 1.0, product_s = 1.0, pi_u, pi_s, pi_0 = 1.0;
    // Check if there is a connection between the search_clause and the variable.
    if (this->AssociatedGraph.Connection(search_clause, variable, connection_type) == -1) {
        return;
    }
    // Get V(search_clause)
    va = this->AssociatedGraph.Clause(search_clause);
    // For every variable j of va (except for the variable)
    for (int j = 0; j < va.size(); j++) {
        if (abs(va[j]) != variable) {
            // Get the va_s (clauses where j appears with the same sign) and
            // v_u (clauses where j appears with the opposite sign) sets.
            if (va[j] > 0)
                this->AssociatedGraph.VariablesInClause(search_clause, va_s, va_u);
            else
                this->AssociatedGraph.VariablesInClause(search_clause, va_u, va_s);
            // Calculation of product u
            for (int b : va_u) {
                product_u *= (1 - this->AssociatedGraph.getEdgeW(b, j));
            }
            // Calculation of product s
            for (int b : va_s) {
                product_s *= (1 - this->AssociatedGraph.getEdgeW(b, j));
            }
            pi_u = (1 - product_u) * product_s;
            pi_s = (1 - product_s) * product_u;
            //Calculation of pi_0
            for (auto b : this->AssociatedGraph.ClausesOfVariable(va[j])) {
                if (b != search_clause) {
                    pi_0 *= 1 - this->AssociatedGraph.getEdgeW(b, j);
                }
            }
            survey *= pi_u / (pi_u + pi_s + pi_0);
            pi_0 = 1.0;
            product_s = 1.0;
            product_u = 1.0;
        } else {
            // Get the index for the setEdgeW function.
            index = j;
        }
    }
    // All the pis are calculated, so we calculate the survey
    this->AssociatedGraph.setEdgeW(search_clause, index, survey);
}

bool SurveyPropagation::SP() {
    bool converged;
    wmatrix prev_warnings;
    std::default_random_engine generator(seed); // Random engine generator.
    std::uniform_real_distribution<double> distribution(0, 1); //Distribution for the random generator.
    uvector clauses_indexes(this->AssociatedGraph.getNClauses());
    clause clause, sel_variables;
    std::iota(clauses_indexes.begin(), clauses_indexes.end(), 0); // Generate the indexes vector.
    for (int iters = 0; iters < this->n_iters; iters++) {
        converged = true;
        prev_warnings = this->AssociatedGraph.getMatrix();
        // Choose random clauses without repetition.
        std::shuffle(clauses_indexes.begin(), clauses_indexes.end(), generator);
        for (int index : clauses_indexes) {
            sel_variables.clear();
            clause = this->AssociatedGraph.Clause(index);
            // Choose random variable from the clause without repetition.
            std::sample(clause.begin(), clause.end(), std::back_inserter(sel_variables), clause.size(), generator);
            // Update every edge.
            for (int i = 0; i < clause.size(); i++) {
                this->Update(index, sel_variables[i] < 0 ? abs(sel_variables[i]) : sel_variables[i]);
            }
        }
        // Check the convergence condition.
        for (int i = 0; i < prev_warnings.size() && converged; i++) {
            for (int j = 0; j < prev_warnings[i].size(); j++) {
                if (std::abs(this->AssociatedGraph.getEdgeW(i, j) - prev_warnings[i][j]) > this->precision) {
                    converged = false;
                    break;
                }
            }
        }
        // If the variable is true, means that all the the edge
        if (converged) {
            std::cout << "Survey propagation has converged" << std::endl;
            return true;
        }
    }
    return false;
}

int SurveyPropagation::SID() {

    // The surveys are randomized by default.

    bool converged = SP();
    if (converged) {
        // Decimate process
    }

}