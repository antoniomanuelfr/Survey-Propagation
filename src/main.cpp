#include <iostream>
#include "SurveyPropagation.h"
int main() {
    FactorGraph mygraph("../../cnf/cnf_5_10.cnf");
    SurveyPropagation sv(mygraph);
    std::vector<int> assignment(mygraph.getNVariables(), 0);
    mygraph.PartialAssignment(assignment);
    std::cout << mygraph << std :: endl;
}
