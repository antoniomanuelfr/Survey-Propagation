//
// Created by antoniomanuelfr on 4/10/20.
//

#ifndef SURVEYPROPAGATION
#define SURVEYPROPAGATION

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
     * @param clause
     * @param variable
     */
    void Update(unsigned int clause, unsigned int variable);

    void print();
};


#endif
