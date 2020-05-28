#include <iostream>
#include "SurveyPropagation.h"
#include "FactorGraph.h"
using namespace std;
int main() {
    FactorGraph my_graph("../../cnf/cnf_100_160.cnf");
    FactorGraph res = my_graph.PartialAssignment(0, true);
    std::cout << res <<  std::endl;
    auto i = res.getUnitVars();
    for (auto it : i)
        cout << "unit var " << it << endl;
    std::pair<int, vector<int>> L;
}
