#include <iostream>
#include "SurveyPropagation.h"
using namespace std;
int main() {
    FactorGraph mygraph("../../cnf/aim-100-1_6-no-1.cnf");
    SurveyPropagation sv(mygraph);
    vector<int> assignment(mygraph.getNVariables(), 0);
    assignment[0] = 1;
    FactorGraph prev = mygraph.PartialAssignment(assignment);
    cout << "Clauses before: " << endl;
    cout << prev.Clause(57) << prev.Clause(58) << prev.Clause(59) << prev.Clause(60);
    cout << "Clauses after: " << endl;
    cout << mygraph.Clause(58);
}
