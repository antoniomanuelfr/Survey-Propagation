#include <iostream>
#include "SurveyPropagation.h"
using namespace std;
int main() {
    FactorGraph my_graph("../../cnf/prueba.cnf");
    std::vector<int> as(my_graph.getNVariables(), 0);
    as[0] = 1;as[1]= 1; as[2] = -1; as[3] = -1;
    FactorGraph res = my_graph.PartialAssignment(as);
    std::cout << res <<  std::endl;
    std::pair<int, vector<int>> L;
    L=my_graph.DPLL(my_graph, -1);
    string a = L.first == SAT ? "satisfacible" : "no satisfacible";
    cout << "L.first = " <<  L.first << std::endl;
    for(auto it : L.second) {
        cout << it << " " << endl;
    }
}
