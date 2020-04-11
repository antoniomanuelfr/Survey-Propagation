//
// Created by antoniomanuelfr on 4/10/20.
//

#include "SurveyPropagation.h"


void SurveyPropagation::print() {
    std::cout << this->AssociatedGraph << std::endl;

}

void SurveyPropagation::Update(unsigned int clause, unsigned int variable) {
    // Preconditions: Clause and variable must be in the range and there has to be a connection.
    if (clause > this->AssociatedGraph.getNClauses() || variable > this->AssociatedGraph.getNVariables()) {
        return;
    }

    bool connection_type;
    std::vector<int> va, va_u, va_s;
    double product_u = 1.0, product_s = 1.0, pi_u = 1.0, pi_s = 1.0, pi_0 = 1.0;
    // Check if there is a connection between the clause and the variable.
    if (this->AssociatedGraph.Connection(clause, variable, connection_type) == -1) {
        return;
    }
    // Get V(clause)
    va = this->AssociatedGraph.Clause(clause);
    // For every variable j of va (except for the variable)
    for (int j : va) {
        if (abs(j) != variable) {
            // Get the va_s (clauses where j appears with the same sign) and
            // v_u (clauses where j appears with the opposite sign) sets.
            if (j > 0)
                this->AssociatedGraph.VariablesInClause(clause, va_s, va_u);
            else
                this->AssociatedGraph.VariablesInClause(clause, va_u, va_s);
            // Calculation of product u
            for (int b : va_u) {
                product_u *= (1 - this->AssociatedGraph.GetEdgeW(b, j));
            }
            // Calculation of product s
            for (int b : va_s) {
                product_s *= (1 - this->AssociatedGraph.GetEdgeW(b, j));
            }
            pi_u = (1 - product_u) * product_s;
            pi_s = (1 - product_s) * product_u;
            //Calculation of pi_0
            for (auto b : this->AssociatedGraph.ClausesOfVariable(j)) {
                if (b != clause) {
                    pi_0 *= 1 - this->AssociatedGraph.GetEdgeW(b, j);
                }
            }
        }
    }
    // All the pis are calculated, so we calculate the survey
    this->AssociatedGraph.SetEdgeW(clause, variable, pi_u / (pi_u + pi_s + pi_0));
}
