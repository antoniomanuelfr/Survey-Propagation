#include <iostream>
#include "SurveyPropagation.h"
int main() {
    FactorGraph mygraph("../../cnf/aim-100-1_6-no-1.cnf");
    SurveyPropagation sv(mygraph);
    sv.print();
}
