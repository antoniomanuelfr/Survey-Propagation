#include <iostream>
#include "SurveyPropagation.h"
using namespace std;
int main() {
    FactorGraph mygraph("../../cnf/cnf_5_10.cnf");
    SurveyPropagation sv(mygraph);
    vector<int> assignment(mygraph.getNVariables(), 0);
    vector<bool> assignment_b(mygraph.getNVariables(), true);
    uvector a;
    assignment_b[0] = false;
    cout << "¿This assignment satisfies the clause? " << mygraph.SatisfiesF(assignment_b, a, 0, <#initializer#>) << endl;
    FactorGraph prev = mygraph.PartialAssignment(assignment);
    cout << "Clauses before: " << endl;
    cout << mygraph.Clause(57) << mygraph.Clause(58) << mygraph.Clause(59) << mygraph.Clause(60);

    cout << "Clauses after: " << endl;
    cout << prev.Clause(57) << prev.Clause(58) << prev.Clause(59) << prev.Clause(60);
    sv.SP();
}
