//
// Created by antoniomanuelfr on 4/10/20.
//

#include "SurveyPropagation.h"


void SurveyPropagation::print() {
    std::cout << this->AssociatedGraph << std::endl;

}

void SurveyPropagation::Update(unsigned int clause, unsigned int variable) {
    bool connection_type;
    std::vector<int> va, va_u, va_s;
    double pi_u = 0.0, pi_s = 0.0, pi_0 = 0.0;
    // Check if there is a connection between the clause and the variable.
    if (this->AssociatedGraph.Connection(clause, variable, connection_type) == -1) {
        return;
    }
    // Get V(clause)
    if (connection_type) {
        va_s = this->AssociatedGraph.getPositiveVariables()[clause];
        va_u = this->AssociatedGraph.getNegativeVariables()[clause];
    } else {
        va_s = this->AssociatedGraph.getNegativeVariables()[clause];
        va_u = this->AssociatedGraph.getPositiveVariables()[clause];
    }


    // Compute pi_u

}
