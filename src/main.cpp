#include <iostream>
#include "SurveyPropagation.h"
using namespace std;
int main() {
    FactorGraph my_graph("../../cnf/aim-100-1_6-no-1.cnf");
    SurveyPropagation sv(my_graph);
    std::vector<bool> assignment;
    sv.SID(assignment);
    std::cout << assignment.size();
}
