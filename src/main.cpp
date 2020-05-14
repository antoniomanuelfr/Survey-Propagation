#include <iostream>
#include "SurveyPropagation.h"
using namespace std;
int main() {
    FactorGraph mygraph("../../cnf/aim-100-1_6-no-1.cnf");
    SurveyPropagation sv(mygraph);
    vector<int> assignment(mygraph.getNVariables(), 0);
    vector<bool> assignment_b(mygraph.getNVariables(), true);
    vector<unsigned int> sat, n_sat;
    uvector a;
    cout << "The satisfied clauses are:" << endl;
    for (auto i : sat) {
        cout << mygraph.Clause(i) << endl;
    }
    cout << "\nThe not satisfied clauses are:" << endl;
    for (auto i : n_sat) {
        cout << mygraph.Clause(i) << endl;
    }
    vector<bool>as = mygraph.WalkSAT(2, 1e5, 0.5, 0);
    cout << as.size() << endl;
}
