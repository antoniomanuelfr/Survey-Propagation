//
// Created by antoniomanuelfr on 4/10/20.
//

#ifndef SURVEY_PROPAGATION_H
#define SURVEY_PROPAGATION_H

#include <utility>

#include "FactorGraph.h"

class SurveyPropagation {
private:
    FactorGraph AssociatedGraph;

public:
    SurveyPropagation(const FactorGraph &AssociatedGraph) {
        this->AssociatedGraph = AssociatedGraph;
    }

    /**
     * @brief Function that implements the SP-Update function.
     * @param search_clause
     * @param variable
     */
    void Update(unsigned int search_clause, unsigned int variable);

    void print();
};


#endif // SURVEY_PROPAGATION_H
