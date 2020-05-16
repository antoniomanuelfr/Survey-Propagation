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
    if (survey < this->lower_bound) {
        survey = 0;
    }
    // All the pis are calculated, so we calculate the survey
    this->AssociatedGraph.setEdgeW(search_clause, index, survey);
}

int SurveyPropagation::SP(bool &trivial) {
    bool converged;
    wmatrix prev_warnings;
    std::default_random_engine generator(seed); // Random engine generator.
    std::uniform_real_distribution<double> distribution(0, 1); //Distribution for the random generator.
    uvector clauses_indexes(this->AssociatedGraph.getNClauses());
    clause clause, sel_variables;
    std::iota(clauses_indexes.begin(), clauses_indexes.end(), 0); // Generate the indexes vector.
    for (int iters = 0; iters < this->n_iters; iters++) {
        converged = true;
        trivial = true;
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
                trivial = trivial ? this->AssociatedGraph.getEdgeW(i, j) == 0 : trivial;
                if (std::abs(this->AssociatedGraph.getEdgeW(i, j) - prev_warnings[i][j]) > this->precision) {
                    converged = false;
                    break;
                }
            }
        }
        // If the variable is true, means that all the the edge
        if (converged) {
            std::cout << "Survey propagation has converged" << std::endl;
            return SP_CONVERGED;
        }
    }
    return SP_UNCONVERGED;
}

void SurveyPropagation::CalculateBiases(std::vector<double> &positive_w, std::vector<double> &negative_w,
                                        std::vector<double> &zero_w, int &max_index) {
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
    int var_index_clause;
    double positive_pi, negative_pi, zero_pi, pos_prod, neg_prod, survey, max = 0.0;

    // For each variable we have to calculate the three pis.
    for (int variable_index = 0; variable_index < this->AssociatedGraph.getNVariables(); variable_index++) {
        pos_prod = 0.0;
        neg_prod = 0.0;
        zero_pi = 0.0;
        // Get the V_+ and V_- sets (clauses where the variable appears as positive and negative
        this->AssociatedGraph.ClausesInVariable(variable_index, positive_clauses, negative_clauses);
        // Positive PI of variable_index
        for (auto it : positive_clauses) {
            actual_clause = this->AssociatedGraph.Clause(it);
            var_index_clause = std::distance(actual_clause.begin(), std::find(actual_clause.begin(), actual_clause.end(), variable_index++));
            survey = this->AssociatedGraph.getEdgeW(it, var_index_clause);
            pos_prod *= (1 - survey);
            zero_pi *= (1 - survey) ;
        }
        // Positive PI of variable_index
        for (auto it : negative_clauses) {
            actual_clause = this->AssociatedGraph.Clause(it);
            int index = std::distance(actual_clause.begin(),std::find(actual_clause.begin(), actual_clause.end(), -(variable_index++)));
            survey = this->AssociatedGraph.getEdgeW(it, index);
            neg_prod *= (1 - survey);
            zero_pi *= (1 - survey);
        }
        positive_pi = (1 - pos_prod) * neg_prod;
        negative_pi = (1 - neg_prod) * pos_prod;

        positive_w[variable_index] = positive_pi / (positive_pi + negative_pi + zero_pi);
        negative_w[variable_index] = negative_pi / (positive_pi + negative_pi + zero_pi);
        zero_w[variable_index] = 1 - positive_w[variable_index] - negative_w[variable_index];
        double difference = std::abs(positive_w[variable_index] - negative_w[variable_index]);
        if (difference > max) {
            max = difference;
            max_index = variable_index;
        }
    }
}

int SurveyPropagation::SID(std::vector<bool> &true_assignment) {
    bool trivial_surveys;
    int max_index;
    std::vector<double> positive_w, negative_w, zero_w;
    std::vector<bool> walksat_assignment;
    std::vector<int>assignment(this->AssociatedGraph.getNVariables(), 0);
    // The surveys are randomized by default.
    if (SP(trivial_surveys) == SP_CONVERGED) {
        // Decimate process, check if the surveys aren't trivial.
        if (!trivial_surveys) {
            this->CalculateBiases(positive_w, negative_w, zero_w, max_index);
            assignment[max_index] = positive_w[max_index] > negative_w[max_index] ? 1 : 0;
            this->AssociatedGraph.PartialAssignment(assignment);
        } else {
            walksat_assignment = this->AssociatedGraph.WalkSAT(
                    this->walksat_iters, this->walksat_flips, this->walksat_noise, this->seed);

            // If WalkSAT has found an assignment, return the assignment.
            if (!walksat_assignment.empty()) {
                true_assignment = walksat_assignment;
                return SAT;
            }
        }
        // Calling unit propagation with the assignment applied.
    } else {
        return SP_UNCONVERGED;
    }
    return PROB_UNSAT;
}


